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
#include "Piece.h"

#include "MinimapGridSystem.h"



bool UIScene::OnCreateScene()
{

    //auto pLine = m_uiManager->CreateWidget<GridLine<Widget>>(L"gridline", XMFLOAT3(7*25, 5*25, 0));
    //pLine->SetScale({ 1, 1, 1 });
    //std::cout << "[UIScene] OnCreate\n";   
    
    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    m_uiManager->SetOrthoFlag(true);
    

    m_uiManager->CreateWidget<Image>(L"tstImg1", XMFLOAT3(0, 0, 0));
    
    m_uiManager->CreateWidget<Image>(L"tstImg2", XMFLOAT3(0, 0, 0));

    // 자식이 있는 경우 체이닝으로 생성 
    // this반환해서 그냥 써도 똑같은 효과
    //m_uiManager->CreateWidget<CardTable>(L"tstCardTable", XMFLOAT3(500, 500, 0))->CreateChild(); 

    //auto* slot0 = m_uiManager->CreateWidget<CardSlot>(L"S0", XMFLOAT3(300, 100, 0));
    //auto* slot1 = m_uiManager->CreateWidget<CardSlot>(L"S1", XMFLOAT3(400, 100, 0));
    //auto* slot2 = m_uiManager->CreateWidget<CardSlot>(L"S2", XMFLOAT3(500, 100, 0));
    //auto* slot3 = m_uiManager->CreateWidget<CardSlot>(L"S3", XMFLOAT3(600, 100, 0));
    //m_uiManager->CreateWidget<Card>(L"tstCard", XMFLOAT3(600, 0, 0));
    //m_uiManager->CreateWidget<Button>(L"tstBtn", XMFLOAT3(400, 0, 0));
    
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");
    

    //m_objectManager->CreateWidget<Image>(L"tst", XMFLOAT3(0, 0, 0));

    auto* a = m_uiManager->CreateWidget<Letterbox>(L"LetterBoxA", XMFLOAT3(0, 0, 0));
    a->SetRole(Letterbox::Role::BarA);
    auto* b = m_uiManager->CreateWidget<Letterbox>(L"LetterBoxB", XMFLOAT3(0, 0, 0));
    b->SetRole(Letterbox::Role::BarB);
    //m_minimap = std::make_unique<MinimapGridSystem>(m_uiManager.get(), m_input);
    //m_minimap->CreateObject(1000, 900, 0);

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

