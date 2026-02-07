#include "pch.h"


#include "RenderTypes.h"
#include "IInput.h"
#include "IRenderer.h"
#include "IWindow.h"
#include "YunoEngine.h"
#include "ISceneManager.h"
#include "YunoCamera.h"

#include "Widget.h"

#include "Title.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "UIScene.h"
#include "PhaseScene.h"
#include "WeaponSelectScene.h"
#include "RenderTest.h"

#include "AudioQueue.h"
#include "PlayQueue.h"

#include "CardManager.h"

#include "GameApp.h"

#include "PacketBuilder.h"


// 패킷들

#include "C2S_Ping.h"
#include "C2S_MatchEnter.h"
#include "C2S_MatchLeave.h"





GameApp::~GameApp() = default;

bool GameApp::OnInit()
{
    std::cout << "[GameApp] OnInit\n";

    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
    {
        std::cout << "[GameApp] Renderer not available.\n";
        return false;
    } // 렌더러 체크


   ISceneManager* sm = YunoEngine::GetSceneManager();
   if (!sm) return false;

   m_gameManager = std::make_unique<GameManager>();
   GameManager::Initialize(m_gameManager.get());
   m_gameManager->BindClientNetwork(&m_clientNet);
   GameManager::Get().Init();

   SceneTransitionOptions opt{};
   opt.immediate = true;
    

   //sm->RequestReplaceRoot(std::make_unique<RenderTest>(), opt);  // 본인이 작업중인 씬으로 넣으면 됨
   //sm->RequestReplaceRoot(std::make_unique<UIScene>(), opt);
   //sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);
   //sm->RequestReplaceRoot(std::make_unique<Title>(), opt);
   sm->RequestReplaceRoot(std::make_unique<PhaseScene>(), opt);



   // UI 재사용 쿼드 제작
   SetupDefWidgetMesh(g_defaultWidgetMesh, renderer);

    // 네트워크 스레드 시작
    m_clientNet.Start("127.0.0.1", 9000);

    m_clientNet.RegisterMatchPacketHandler();



    return true;
}

void GameApp::OnUpdate(float dt)
{
    m_gameManager->Tick(dt);
    m_clientNet.PumpIncoming(dt);

    static float acc = 0.0f;
    static int frameCount = 0;

    acc += dt;
    ++frameCount;

    CameraMove(dt);

    // MSAA 변경되는지 테스트
    //static float test = 0.0f;
    //test += dt;
    //
    //if (test >= 4.0f) 
    //{
    //    IRenderer* renderer = YunoEngine::GetRenderer();
    //    int msaaVal = renderer->GetMSAASamples();
    //    std::cout <<"before msaa Value = " << msaaVal;
    //    if(msaaVal == 1)
    //        renderer->SetMSAASamples(8);
    //    else
    //        renderer->SetMSAASamples(1);
    //
    //    msaaVal = renderer->GetMSAASamples();
    //    std::cout << ", After msaa Value = " << msaaVal<<std::endl;
    //
    //    test = 0.0f;
    //}
    IRenderer* renderer = YunoEngine::GetRenderer();
    IInput* input = YunoEngine::GetInput();
    IWindow* window = YunoEngine::GetWindow();
    ISceneManager* sm = YunoEngine::GetSceneManager();
    IAudioManager* am = YunoEngine::GetAudioManager();

    if (input->IsKeyDown('O')) // >> 이거 인스턴스 호출해서 키다운하는거 불편하니까 나중에 바꾸기 ㄱㄱ
        window->SetClientSize(1920, 1080);

    if (input->IsKeyDown('P'))
        window->SetClientSize(3440, 1440);


    if (input && sm)
    {


        //if (input->IsKeyPressed(VK_F2))
        //{
        //    SceneTransitionOptions opt{};
        //    opt.immediate = true;
        //    sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);
        //    //sm->RequestPush(std::make_unique<PlayScene>());
        //}

        //if (input->IsKeyPressed(VK_F3))
        //{
        //    SceneTransitionOptions opt{};
        //    opt.immediate = true;
        //    sm->RequestReplaceRoot(std::make_unique<UIScene>(), opt);
        //    //sm->RequestPush(std::make_unique<PlayScene>());
        //}
        //
        //if (input->IsKeyPressed(VK_F4))
        //{
        //    SceneTransitionOptions opt{};
        //    opt.immediate = true;
        //    ScenePolicy policy;
        //    policy.blockUpdateBelow = false;
        //    policy.blockRenderBelow = false;
        //    //sm->RequestReplaceRoot(std::make_unique<TitleScene>(), opt);
        //    sm->RequestPush(std::make_unique<TitleScene>(), policy);
        //}
        //
        //if (input->IsKeyPressed(VK_F5))
        //{
        //    SceneTransitionOptions opt{};
        //    opt.immediate = true;
        //    ScenePolicy policy;
        //    policy.blockUpdateBelow = false;
        //    policy.blockRenderBelow = false;
        //    //sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);
        //    sm->RequestPush(std::make_unique<PlayScene>(), policy);
        //}
        //
        //if (input->IsKeyPressed(VK_F6))
        //{
        //    SceneTransitionOptions opt{};
        //    opt.immediate = true;
        //    ScenePolicy policy;
        //    policy.blockUpdateBelow = false;
        //    policy.blockRenderBelow = false;
        //    //sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);
        //    sm->RequestPush(std::make_unique<UIScene>(), policy);
        //}

        //if (input->IsKeyPressed(VK_OEM_PERIOD))
        //{
        //    AudioQ::Insert(AudioQ::StopOrRestartEvent(EventName::BGM_Playlist, true));
        //    AudioQ::Insert(AudioQ::StopOrRestartEvent(EventName::BGM_Playlist, false));
        //}

        if (input->IsKeyPressed('1'))
        {
            using namespace yuno::net;
            yuno::net::packets::C2S_Ping ping{};
            ping.nonce = 1000;

            auto bytes = PacketBuilder::Build(
                PacketType::C2S_Ping,
                [&](ByteWriter& w)
                {
                    ping.Serialize(w);
                });

            m_clientNet.SendPacket(std::move(bytes));
        }

    }

    // audio-> StateCheck();



    //if (acc >= 1.0f)
    //{
    //    std::cout << "[GameApp] dt = " << dt << "\n";
    //    const float fps = static_cast<float>(frameCount) / acc;
    //    std::cout << "[GameApp] FPS = " << fps << "\n";
    //    acc = 0.0f;
    //    frameCount = 0;
    //}



    am->Update(dt);
}

void GameApp::OnFixedUpdate(float fixedDt)
{
    static int step = 0;
    ++step;

    //if (step % 60 == 0)
    //{
    //    std::cout << "[GameApp] FixedUpdate dt = " << fixedDt << "\n";
    //}
}

void GameApp::OnShutdown()
{
    std::cout << "[GameApp] OnShutdown\n";

    GameManager::Shutdown();
    m_gameManager.reset();

    // 네트워크 스레드종료
    m_clientNet.Stop();

    //if (m_net)
    //{
    //    m_net->Close();
    //    m_net.reset();
    //}
}


void CameraMove(float dt)
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    IInput* input = YunoEngine::GetInput();

    static bool s_cameraInitialized = false;
    static float s_cameraYaw = 0.0f;
    static float s_cameraPitch = 0.0f;

    YunoCamera& camera = renderer->GetCamera();
    XMVECTOR position = XMLoadFloat3(&camera.position);

    if (!s_cameraInitialized)
    {
        XMVECTOR toTarget = XMVectorSubtract(XMLoadFloat3(&camera.target), position);
        float lenSq = XMVectorGetX(XMVector3LengthSq(toTarget));
        if (lenSq < 0.0001f)
        {
            toTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        }
        XMVECTOR forward = XMVector3Normalize(toTarget);
        XMFLOAT3 forwardFloat{};
        XMStoreFloat3(&forwardFloat, forward);
        s_cameraYaw = atan2f(forwardFloat.x, forwardFloat.z);
        s_cameraPitch = asinf(forwardFloat.y);
        s_cameraInitialized = true;
    }

    const float lookSpeed = 0.005f;
    if (input->IsMouseButtonDown(1))
    {
        s_cameraYaw += input->GetMouseDeltaX() * lookSpeed;
        s_cameraPitch -= input->GetMouseDeltaY() * lookSpeed;

        const float maxPitch = XM_PIDIV2 - 0.01f;
        if (s_cameraPitch > maxPitch) s_cameraPitch = maxPitch;
        if (s_cameraPitch < -maxPitch) s_cameraPitch = -maxPitch;
    }

    XMVECTOR forward = XMVectorSet(
        cosf(s_cameraPitch) * sinf(s_cameraYaw),
        sinf(s_cameraPitch),
        cosf(s_cameraPitch) * cosf(s_cameraYaw),
        0.0f
    );
    XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));

    XMVECTOR move = XMVectorZero();
    if (input->IsKeyDown('W')) move = XMVectorAdd(move, forward);
    if (input->IsKeyDown('S')) move = XMVectorSubtract(move, forward);
    if (input->IsKeyDown('A')) move = XMVectorSubtract(move, right);
    if (input->IsKeyDown('D')) move = XMVectorAdd(move, right);
    if (input->IsKeyDown('Q')) move = XMVectorSubtract(move, worldUp);
    if (input->IsKeyDown('E')) move = XMVectorAdd(move, worldUp);

    const float moveSpeed = 10.0f;
    float moveLenSq = XMVectorGetX(XMVector3LengthSq(move));
    if (moveLenSq > 0.0001f)
    {
        move = XMVector3Normalize(move);
        position = XMVectorAdd(position, XMVectorScale(move, moveSpeed * dt));
    }

    XMVECTOR target = XMVectorAdd(position, forward);
    XMStoreFloat3(&camera.position, position);
    XMStoreFloat3(&camera.target, target);
    camera.up = { 0.0f, 1.0f, 0.0f };
}



