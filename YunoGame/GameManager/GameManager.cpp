#include "pch.h"

#include "GameManager.h"

#include "YunoEngine.h"
#include "ISceneManager.h"

#include "Title.h"
#include "TitleScene.h"
#include "WeaponSelectScene.h"
#include "PlayScene.h"
#include "PhaseScene.h"

#include "YunoClientNetwork.h"

//패킷
#include "PacketBuilder.h"
#include "C2S_BattlePackets.h"
#include "C2S_ReadySet.h"
#include "C2S_CardPackets.h"
#include "C2S_MatchLeave.h"
#include "C2S_Emote.h"
GameManager* GameManager::s_instance = nullptr;


void GameManager::SetWeaponData(int _pId, int _slotId, int _weaponId, int _hp, int _stamina, int _currentTile)
{
    Wdata data;
    data.pId = _pId;
    data.slotId = _slotId;
    data.weaponId = _weaponId;
    data.hp = _hp;
    data.stamina = _stamina;
    data.currentTile = _currentTile;

    m_weapons.push_back(data);
}

void GameManager::PushBattlePacket(const BattleResult _BattleResult)
{
    m_turnPkts.push(_BattleResult);
    std::cout << "Battle Packet is inserted into Queue.\n";
}

BattleResult GameManager::PopBattlePacket()
{
    BattleResult pckt = m_turnPkts.front();
    m_turnPkts.pop();
    std::cout << "Battle Packet is popped out of Queue.\n";
    return pckt;
}

bool GameManager::IsEmptyBattlePacket()
{
    return m_turnPkts.empty();
}


void GameManager::Initialize(GameManager* inst)
{
    assert(inst);
    assert(!s_instance);
    s_instance = inst;

}

void GameManager::Init()
{
    m_cardBasicMng.LoadFromCSV("../Assets/CardData/CardData.csv");
    m_cardRangeMng.LoadFromCSV("../Assets/CardData/CardRange.csv");
}

void GameManager::Shutdown()
{
    assert(s_instance);
    s_instance = nullptr;
}

GameManager& GameManager::Get()
{
    assert(s_instance);
    return *s_instance;
}

void GameManager::SetSceneState(CurrentSceneState state)
{
    ISceneManager* sm = YunoEngine::GetSceneManager();
    if (!sm) return;

    switch (state) 
    {
    case CurrentSceneState::Title:
    {
        m_state = CurrentSceneState::Title;
        SceneTransitionOptions opt{};
        opt.immediate = false;
        sm->RequestReplaceRoot(std::make_unique<Title>(), opt);
        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::C2S_MatchLeave,
            [&](yuno::net::ByteWriter& w)
            {
                yuno::net::packets::C2S_MatchLeave req{};
                req.Serialize(w);
            });

        GameManager::Get().SendPacket(std::move(bytes));

        break;
    }
    case CurrentSceneState::GameStart:
    {
        m_state = CurrentSceneState::GameStart;
        SceneTransitionOptions opt{};
        opt.immediate = false;
        sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);
        // 씬에 관련된 데이터들을 같이 넘길거야
        break;
    }
    case CurrentSceneState::CountDown:
    {

        std::cout << "3...2...1...Battle Start!!!!!" << std::endl;  

        //SetSceneState(CurrentSceneState::RoundStart);

        break;
    }
    case CurrentSceneState::RoundStart:
    {
        // 이미 라운드 씬이면 무시
        //if (m_state == CurrentSceneState::RoundStart) return;
        m_state = CurrentSceneState::RoundStart;
        SceneTransitionOptions opt{};
        opt.immediate = true;

        sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);

        SetSceneState(CurrentSceneState::SubmitCard);
        break;
    }
    case CurrentSceneState::SubmitCard:
    {
        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = false;

        sm->RequestPush(std::make_unique<PhaseScene>(), sp);

        break;
    }
    case CurrentSceneState::AutoBattle:
    {
        m_state = CurrentSceneState::AutoBattle;

        break;
    }
    case CurrentSceneState::RoundEnd:
    {
        break;
    }
    case CurrentSceneState::GameEnd:
    {
        break;
    }
    default:
        assert(false,"No state Scene");



    }
}

void GameManager::InitHands(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    for (int i = 0; i < 2; ++i)
    {
        m_myHands[i].cards.clear();
        m_enemyHands[i].cards.clear();
    }

    m_CardRuntimeIDs.clear();

    AddCards(cards);
}

void GameManager::AddCards(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    for (const auto& card : cards)
    {
        ClientCardInfo info{card.runtimeID, card.dataID };


        bool isMine = (card.PID == m_PID); // PID = 1,2
        int unitIdx = card.slotID - 1;
        
        if(unitIdx < 0 || unitIdx >= 2)
            continue;

        if (isMine)
            m_myHands[unitIdx].cards.push_back(info);
        else
            m_enemyHands[unitIdx].cards.push_back(info);

        m_CardRuntimeIDs[card.runtimeID] = card.dataID;
    }
}

void GameManager::RemoveCard(uint32_t runtimeID)
{
    for (int u = 0; u < 2; ++u)
    {
        auto& hand = m_myHands[u].cards;
        auto it = std::find_if(hand.begin(), hand.end(),
            [&](const ClientCardInfo& c)
            {
                return c.runtimeID == runtimeID;
            });

        if (it != hand.end())
        {
            hand.erase(it);
            break;
        }
    }

    m_CardRuntimeIDs.erase(runtimeID);
}

uint32_t GameManager::GetMyCardRuntimeID(int unitSlot, int index) const
{
    if (unitSlot < 0 || unitSlot >= 2)
        return 0;

    const auto& hand = m_myHands[unitSlot].cards;

    if (index < 0)
        return 0;

    return hand[index].runtimeID;
}

uint32_t GameManager::GetCardDataID(uint32_t runtimeID) const
{
    auto it = m_CardRuntimeIDs.find(runtimeID);
    if (it == m_CardRuntimeIDs.end())
        return 0;
    
    return it->second;
}

const CardData GameManager::GetCardData(uint32_t runtimeID)
{
    auto dataID = GetCardDataID(runtimeID);
    return m_cardBasicMng.GetCardData(dataID);
}

const CardEffectData* GameManager::GetCardEffectData(uint32_t runtimeID)
{
    auto dataID = GetCardDataID(runtimeID);
    return m_cardBasicMng.GetEffectData(dataID);
}

const RangeData* GameManager::GetRangeData(uint32_t runtimeID)
{
    auto dataID = GetCardData(runtimeID);
    return m_cardRangeMng.GetRange(dataID.m_rangeId);
}

void GameManager::SetDrawCandidates(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    m_drawCandidates.clear();

    for (const auto& c : cards)
    {
        m_drawCandidates.push_back({
            c.runtimeID,
            c.dataID
            });
    }

    std::cout << "[Client] DrawCandidates stored. count="
        << m_drawCandidates.size() << "\n";
}
void GameManager::SendSelectCard(int index)
{
    if (index < 0 || index >= m_drawCandidates.size())
        return;

    uint32_t runtimeID = m_drawCandidates[index].runtimeID;

    yuno::net::packets::C2S_SelectCard pkt{};
    pkt.runtimeID = runtimeID;

    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_SelectCard,
        [&](yuno::net::ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));
}
const std::vector<ClientCardInfo>& GameManager::GetDrawCandidates() const
{
    return m_drawCandidates;
}
void GameManager::ClearDrawCandidates()
{
    m_drawCandidates.clear();
}

bool GameManager::PushCardCommand(const CardPlayCommand& cmd)
{
    return m_cardQueue.Push(cmd);
}

void GameManager::ClearCardQueue()
{
    m_cardQueue.Clear();
}

bool GameManager::IsCardQueueEmpty() const
{
    return m_cardQueue.IsEmpty();
}

bool GameManager::IsCardQueueFull() const
{
    return m_cardQueue.IsFull();
}

const std::vector<CardPlayCommand>& GameManager::GetCardQueue() const
{
    return m_cardQueue.Get();
}

void GameManager::SendEmote(uint8_t emoteId)
{
    yuno::net::packets::C2S_Emote pkt{};
    pkt.emoteId = emoteId;

    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_Emote,
        [&](yuno::net::ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));

    std::cout << "[Client] Send Emote emoteId=" << int(emoteId) << "\n";
}

void GameManager::PushEmote(uint8_t pid, uint8_t emoteId)
{
    m_pendingEmotes.push({ pid, emoteId });
}

bool GameManager::PopEmote(PendingEmote& out)
{
    if (m_pendingEmotes.empty())
        return false;

    out = m_pendingEmotes.front();
    m_pendingEmotes.pop();
    return true;
}

void GameManager::StartCountDown(int countTime, int S1U1, int S1U2, int S2U1, int S2U2)
{
    // 상대방 슬롯에 전달받은 유닛ID에 맞는 텍스쳐로 변경하기 넣으면 됨
    // + 카운트 다운 3초 시작
    // 3초 후에 RoundStart씬으로 이동하면서 데이터초기화
    m_S1U1 = S1U1;
    m_S1U2 = S1U2;
    m_S2U1 = S2U1;
    m_S2U2 = S2U2;

    m_countdownActive = true;
    m_countdownRemaining = static_cast<float>(countTime);

    SetSceneState(CurrentSceneState::CountDown);
}

void GameManager::Tick(float dt)
{
    if (m_countdownActive)
    {
        m_countdownRemaining -= dt;
        if (m_countdownRemaining <= 0.0f)
        {
            m_countdownActive = false;
            m_countdownRemaining = 0.0f;
    
            SetSceneState(CurrentSceneState::RoundStart);
        }
    }
}

void GameManager::SendPacket(std::vector<std::uint8_t>&& bytes)
{
    if (!m_clientNet)
    {
        std::cout << "[GameManager] Client network not bound.\n";
        return;
    }

    m_clientNet->SendPacket(std::move(bytes));
}

void GameManager::SetMyPick(int index, PieceType type)
{
    if (index < 0 || index >= 2)
        return;

    m_myPick[index] = type;
}

bool GameManager::HasTwoPicks() const
{
    return (m_myPick[0] != PieceType::None) && (m_myPick[1] != PieceType::None);
}

bool GameManager::ToggleReady()
{
    m_isReady = !m_isReady;
    return m_isReady;
}

void GameManager::SubmitTurn(
    const std::vector<CardPlayCommand>& commands)
{
    using namespace yuno::net;

    packets::C2S_ReadyTurn pkt;
    pkt.commands = commands;

    auto bytes = PacketBuilder::Build(
        PacketType::C2S_ReadyTurn,
        [&](ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));
}


//void GameManager::RoundInit(yuno::net::packets::S2C_Error data)
//{
//    // 너가 패킷 사용해서 하고싶은거하면돼
//    // 데이터 어디 담아두는 멤버 만들어서 저장만했다가
//    // 그리드 GM.GetErrorPkt();
//    //
//}
