#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "Title.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"
// 게임 매니저
#include "GameManager.h"
#include "UIManager.h"

#include"IWindow.h"

// 여러 오브젝트들 ;; 
#include "UIWidgets.h"

// 사용법
// 컨트롤 + H 누르면 이름 변경 나옴
// 위쪽 칸에 Title 적고
// 아래 칸에 원하는 씬 이름 적고
// 모두 바꾸기 누르면 됨 .h 파일도 동일하게 ㄱㄱ

bool Title::OnCreateScene()
{


    m_uiManager->SetOrthoFlag(true);
    auto iwindow = YunoEngine::GetWindow();
    float ClientW = static_cast<float>(iwindow->GetClientWidth());
    float ClientH = static_cast<float>(iwindow->GetClientHeight());


    CreateWidget<TextureImage>(L"Title", L"../Assets/UI/TITLE/Background.png", XMFLOAT3(0, 0, 0));

    m_startBtn = CreateWidget<SceneChangeButton>(L"StartBtn", Float2(1538, 105), XMFLOAT3(ClientW/2, ClientH/2, 0), UIDirection::Center);       // 나중에 1920 1080 <-> 960 540 정상화되면 /2 ㄱㄱ
    m_startBtn->SetTargetScene(CurrentSceneState::GameStart);
    m_startBtn->SetCursurTexture(L"../Assets/UI/TITLE/start_mouseout.png", L"../Assets/UI/TITLE/start_mouseover.png");

    CreateWidget<ExitButton>(L"ExitBtn", Float2(1538, 105), XMFLOAT3(ClientW/2, ClientH/2+150, 0), UIDirection::Center)->SetCursurTexture(L"../Assets/UI/TITLE/exit_mouseout.png", L"../Assets/UI/TITLE/exit_mouseover.png");
    return true;
}

void Title::OnDestroyScene()
{
    //std::cout << "[Title] OnDestroy\n";

}

void Title::OnEnter()
{
    //std::cout << "[Title] OnEnter\n"; 
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void Title::OnExit()
{
    //std::cout << "[Title] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void Title::Update(float dt)
{
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);
}

void Title::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void Title::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
