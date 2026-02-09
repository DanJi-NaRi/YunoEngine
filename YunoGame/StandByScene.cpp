#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "StandByScene.h"
// 다음 엔진
#include "YunoEngine.h"
#include "iwindow.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"

// 게임 매니저
#include "GameManager.h"
#include "UIManager.h"

// 여러 오브젝트들 ;; 
#include "UIWidgets.h"


// 사용법
// 컨트롤 + H 누르면 이름 변경 나옴
// 위쪽 칸에 StandByScene 적고
// 아래 칸에 원하는 씬 이름 적고
// 모두 바꾸기 누르면 됨 .h 파일도 동일하게 ㄱㄱ

bool StandByScene::OnCreateScene()
{
    auto iwindow = YunoEngine::GetWindow();
    float ClientW = static_cast<float>(iwindow->GetClientWidth());
    float ClientH = static_cast<float>(iwindow->GetClientHeight());

    m_uiManager->SetOrthoFlag(true);

    CreateWidget<TextureImage>(L"StandByBackGround", L"../Assets/UI/WEAPON_SELECT/black_background.png", XMFLOAT3(ClientW / 2, ClientH / 2, 0), UIDirection::Center);
    m_pStandByBackImage = CreateWidget<TextureImage>(L"StandByBackGround", L"../Assets/UI/WEAPON_SELECT/waiting_background.png", XMFLOAT3(ClientW / 2, ClientH / 2, 0), UIDirection::Center);
    m_pStandByImage = CreateWidget<TextureImage>(L"StandByImage", L"../Assets/UI/WEAPON_SELECT/waiting_1.png", XMFLOAT3(ClientW / 2, ClientH / 2, 0), UIDirection::Center);
    
    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    // 직교투영 필요한 씬만 ㄱㄱ
    //m_uiManager->SetOrthoFlag(true);

    // Sample Object 생성 예시
    //m_objectManager->CreateObject<ObjectClass>(L"-name-", XMFLOAT3(0, 0, 0));

    // FBX 파일로부터 오브젝트 생성 예시
    //m_objectManager->CreateObjectFromFile<ObjectClass>(L"-name-", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    return true;
}

void StandByScene::OnDestroyScene()
{
    //std::cout << "[StandByScene] OnDestroy\n";

}

void StandByScene::OnEnter()
{
    //std::cout << "[StandByScene] OnEnter\n"; 
}

void StandByScene::OnExit()
{
    //std::cout << "[StandByScene] OnExit\n"; 
}

void StandByScene::Update(float dt)
{
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);
}

void StandByScene::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void StandByScene::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
