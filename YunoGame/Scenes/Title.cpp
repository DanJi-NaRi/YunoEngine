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

// 여러 오브젝트들 ;; 
#include "UIWidgets.h"

// 사용법
// 컨트롤 + H 누르면 이름 변경 나옴
// 위쪽 칸에 Title 적고
// 아래 칸에 원하는 씬 이름 적고
// 모두 바꾸기 누르면 됨 .h 파일도 동일하게 ㄱㄱ

bool Title::OnCreateScene()
{
    //std::cout << "[Title] OnCreate\n";

    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    // 직교투영 필요한 씬만 ㄱㄱ
    m_uiManager->SetOrthoFlag(true);

    // Sample Object 생성 예시
    //m_objectManager->CreateObject<ObjectClass>(L"-name-", XMFLOAT3(0, 0, 0));

    // FBX 파일로부터 오브젝트 생성 예시
    //m_objectManager->CreateObjectFromFile<ObjectClass>(L"-name-", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    m_titleImage = CreateWidget<TitleImage>(L"Title", g_defWidgetSize, XMFLOAT3(0, 0, 0));
    m_titleImage->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));


    CreateWidget<SceneChangeButton>(L"StartBtn", Float2(500,100), XMFLOAT3(100, 200, 0))->SetTargetScene(CurrentSceneState::GameStart);

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
