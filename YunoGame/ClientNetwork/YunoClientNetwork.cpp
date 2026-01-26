#include "pch.h"

#include "YunoClientNetwork.h"

// 이거로 게임 관리할거임
#include "GameManager.h"

// 패킷들
#include "S2C_Pong.h"
#include "S2C_EnterOK.h"
#include "S2C_Error.h"
#include "S2C_CountDown.h"




namespace yuno::game
{
    YunoClientNetwork::YunoClientNetwork()
        : m_workGuard(boost::asio::make_work_guard(m_io))
        , m_client(m_io)
    {
        m_gameManager = std::make_unique<GameManager>();

        m_serverPeer.sId = 0;

        m_client.SetOnPacket(
            [this](std::vector<std::uint8_t>&& packet)
            {
                // 이 콜백은 io_context 스레드에서 호출됨
                PushIncoming(std::move(packet));
            });

        m_client.SetOnDisconnected(
            [this](const boost::system::error_code& /*ec*/)
            {
                // 필요하면 "끊김 이벤트"를 별도 큐로도 올릴 수 있음
            });
    }

    YunoClientNetwork::~YunoClientNetwork()
    {
        Stop();
    }

    void YunoClientNetwork::Start(const std::string& host, std::uint16_t port)
    {
        if (m_running.exchange(true))
            return;

        // io_context 실행 스레드 시작
        m_netThread = std::thread(
            [this]()
            {
                m_io.run();
            });

        // 연결 시도는 네트워크 스레드 컨텍스트에서 실행되도록 post
        boost::asio::post(
            m_io,
            [this, host, port]()
            {
                m_client.Connect(host, port);
            });
    }

    void YunoClientNetwork::Stop()
    {
        if (!m_running.exchange(false))
            return;

        // Disconnect도 io 스레드에서 처리
        boost::asio::post(
            m_io,
            [this]()
            {
                m_client.Disconnect();
            });

        m_workGuard.reset();
        m_io.stop();

        if (m_netThread.joinable())
            m_netThread.join();

        // 큐 정리
        {
            std::lock_guard<std::mutex> lock(m_inMtx);
            m_inQ.clear();
        }
    }

    bool YunoClientNetwork::IsConnected() const
    {
        return m_client.IsConnected();
    }

    void YunoClientNetwork::SendPacket(std::vector<std::uint8_t> packetBytes)
    {
        boost::asio::post(
            m_io,
            [this, bytes = std::move(packetBytes)]() mutable
            {
                m_client.Send(std::move(bytes));
            });
    }

    void YunoClientNetwork::PumpIncoming(float dt)
    {
        m_gameManager->Tick(dt);
        // 메인 스레드에서만 호출
        std::vector<std::uint8_t> pkt;
        while (PopIncoming(pkt))
        {
            m_dispatcher.Dispatch(m_serverPeer, pkt);
        }
    }

    void YunoClientNetwork::PushIncoming(std::vector<std::uint8_t>&& packetBytes)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        m_inQ.emplace_back(std::move(packetBytes));
    }

    bool YunoClientNetwork::PopIncoming(std::vector<std::uint8_t>& out)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        if (m_inQ.empty())
            return false;

        out = std::move(m_inQ.front());
        m_inQ.pop_front();
        return true;
    }


    // ------------------------------- 핸들 함수 등록 -------------------------------
    void YunoClientNetwork::RegisterMatchPacketHandler() 
    {

        using namespace yuno::net;

        Dispatcher().RegisterRaw(
            PacketType::S2C_Pong,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                if (bodyLen < 4)
                    return;

                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_Pong::Deserialize(r);
                std::cout << "InComing Data : " << pkt.nonce << std::endl;

            });

        Dispatcher().RegisterRaw(
            PacketType::S2C_EnterOK,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_EnterOK::Deserialize(r);

                std::cout << "Slot Idx : " << static_cast<int>(pkt.slotIndex) << ", Player Count : " << static_cast<int>(pkt.playerCount) << std::endl;

                m_gameManager->SetSceneState(CurrentSceneState::GameStart);

                //SceneTransitionOptions opt{};
                //opt.immediate = true;
                //sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);

            });

        Dispatcher().RegisterRaw(
            PacketType::S2C_CountDown,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_CountDown::Deserialize(r);


                m_gameManager->StartCountDown(pkt.countTime,pkt.slot1_UnitId1,pkt.slot1_UnitId2,pkt.slot2_UnitId1,pkt.slot2_UnitId2);   // 이렇게만해도 로컬환경이기 때문에 어지간해선 동기화 맞지 않을까....
                //m_gameManager->SetSceneState(CurrentSceneState::RoundStart);

                //SceneTransitionOptions opt{};
                //opt.immediate = true;
                //sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);

            });

        Dispatcher().RegisterRaw(
            PacketType::S2C_Error,
            [this](const NetPeer& /*peer*/,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen < 4)
                {
                    std::cout << "[Client] S2C_Error invalid bodyLen=" << bodyLen << "\n";
                    return;
                }
                 
                ByteReader r(body, bodyLen);
                const auto err = yuno::net::packets::S2C_Error::Deserialize(r);

                const auto code = err.code;         // 어떤 에러가 났는지
                const auto reason = err.reason;     // 왜 발생했는지
                const auto ctx = err.contextType;   // 어떤 패킷에서 발생했는지


                // MatchEnter 거부 처리
                if (code == yuno::net::packets::ErrorCode::EnterDenied &&
                    ctx == PacketType::C2S_MatchEnter)
                {

                    if (reason == yuno::net::packets::EnterDeniedReason::RoomFull)
                    {
                        std::cout << "[Client] Enter denied: RoomFull\n";

                        return;
                    }
                    else if (reason == yuno::net::packets::EnterDeniedReason::AlreadyInMatch)
                    {
                        std::cout << "[Client] Enter denied: AlreadyInMatch\n";

                        return;
                    }

                    return;
                }

            }
        );
    }
}
