#include "pch.h"

#include "UIScene.h"

#include "YunoEngine.h"
#include "YunoCamera.h"

#include "ObjectManager.h"
#include "UIManager.h"
//#include "UI_InputContext.h"
#include "IInput.h"
#include "YunoLight.h"

#include "UIWidgets.h"

#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"

// 테스트 중
//#include "GridLine.h"
//#include "PlayQueue.h"
#include "MinimapQueue.h"
#include "MinimapGridSystem.h"



bool UIScene::OnCreateScene()
{
    // 테스트 공간
    m_uiManager->SetOrthoFlag(true);
    //auto clientX = YunoEngine::GetWindow()->GetClientWidth();
    //auto clientY = YunoEngine::GetWindow()->GetClientHeight();

    const auto clientX = g_DefaultClientXY.x;
    const auto clientY = g_DefaultClientXY.y;
    /*CreateWidget<PlayerIcon>(L"tstPlayerIconL", Float2(217, 188), XMFLOAT3(700, 500, 0), UIDirection::Center);
    
    auto* IconR = CreateWidget<PlayerIcon>(L"tstPlayerIconR", Float2(-217, 188), XMFLOAT3(700, 500, 0), UIDirection::Center);
    IconR->MirrorScaleX();*/


    CreateWidget<Minimap>(L"Minimap", Float2(730, 616), XMFLOAT3(clientX * 0.5f, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::Bottom);

    auto* a = CreateWidget<Letterbox>(L"LetterBoxA", Float2(50, 50), XMFLOAT3(0, 0, 0) );
    a->SetRole(Letterbox::Role::BarA);
    auto* b = CreateWidget<Letterbox>(L"LetterBoxB", Float2(50, 50), XMFLOAT3(0, 0, 0));
    b->SetRole(Letterbox::Role::BarB);

    return true;
}

void UIScene::OnDestroyScene()
{
    //std::cout << "[UIScene] OnDestroy\n";

}

void UIScene::TestInput()
{
    // 테스트용 -> ally1으로 부여한 기물이 움직여용
    if (m_input->IsKeyPressed(0x31))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 0, 3));   // 왼쪽
    }
    if (m_input->IsKeyPressed(0x32))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 1, 2));   // 아래
    }
    if (m_input->IsKeyPressed(0x33))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 2, 3));   // 위
    }
    if (m_input->IsKeyPressed(0x34))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 1, 4));   // 오른쪽
    }
    if (m_input->IsKeyPressed(0x35))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 0, 2));   // 왼쪽 위
    }
    if (m_input->IsKeyPressed(0x36))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 2, 2));   // 오른쪽 위
    }
    if (m_input->IsKeyPressed(0x37))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 0, 4));   // 왼쪽 아래
    }
    if (m_input->IsKeyPressed(0x38))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 2, 4));   // 오른쪽 아래
    }
    if (m_input->IsKeyPressed(0x39))
    {
        MinimapQ::Insert(MinimapQ::Attack_S_TST(GamePiece::Ally1));   // 공격
    }
    if (m_input->IsKeyPressed(0x30))
    {
        MinimapQ::Insert(MinimapQ::Move_S(GamePiece::Ally1, 1, 3));   // 충돌
    }
}


void UIScene::OnEnter()
{
    //std::cout << "[UIScene] OnEnter\n"; 
    // 컨텍스트 추가
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void UIScene::OnExit()
{
    //std::cout << "[UIScene] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}



void UIScene::Update(float dt)
{
    //m_uiManager->GetCursurStstem()->UpdateCheckSnap();
    SceneBase::Update(dt); // 여기만 UI 출력하게끔 빼둘까?
    TestInput();
    //m_input->Dispatch();
    //m_minimap->Update(dt);
}

void UIScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void UIScene::SubmitUI()
{
    SceneBase::SubmitUI();
}

