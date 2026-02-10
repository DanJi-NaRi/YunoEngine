#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "PlayHUDScene.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"
// 게임 매니저
#include "GameManager.h"
#include "UIManager.h"
#include "iwindow.h"

// 여러 오브젝트들 ;; 
#include "UIWidgets.h"



bool PlayHUDScene::OnCreateScene()
{
    auto iwindow = YunoEngine::GetWindow();
    float ClientW = static_cast<float>(iwindow->GetClientWidth());
    float ClientH = static_cast<float>(iwindow->GetClientHeight());

    m_uiManager->SetOrthoFlag(true);

    // BackGround
    CreateWidget<TextureImage>(L"BackGround", L"../Assets/UI/PLAY/Play_background_fade.png", XMFLOAT3(0, 0, 0));

    // Player
    CreateWidget<TextureImage>(L"P1", L"../Assets/UI/PLAY/Play_background_1P_name.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"P2", L"../Assets/UI/PLAY/Play_background_2P_name.png", XMFLOAT3(0, 0, 0));

    // Turn
    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Play_background_turn.png", XMFLOAT3(0, 0, 0));


    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Round_0.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Round_1.png", XMFLOAT3(0, 0, 0));

    return true;
}

void PlayHUDScene::OnDestroyScene()
{
    //std::cout << "[PlayHUDScene] OnDestroy\n";

}

void PlayHUDScene::OnEnter()
{
    //std::cout << "[PlayHUDScene] OnEnter\n"; 
}

void PlayHUDScene::OnExit()
{
    //std::cout << "[PlayHUDScene] OnExit\n"; 
}

void PlayHUDScene::Update(float dt)
{
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);
}

void PlayHUDScene::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void PlayHUDScene::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
