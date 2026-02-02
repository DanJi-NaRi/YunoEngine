#include "pch.h"

#include "PhaseScene.h"

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



bool PhaseScene::OnCreateScene()
{

    //auto pLine = m_uiManager->CreateWidget<GridLine<Widget>>(L"gridline", XMFLOAT3(7*25, 5*25, 0));
    //pLine->SetScale({ 1, 1, 1 });
    //std::cout << "[UIScene] OnCreate\n";   

    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    m_uiManager->SetOrthoFlag(true);

    // m_uiManager->CreateWidget<Image>(L"tstImg1", XMFLOAT3(500, 500, 0));

     //m_uiManager->CreateWidget<Image>(L"tstImg2", XMFLOAT3(510, 510, 0));
    auto clientX = YunoEngine::GetWindow()->GetClientWidth();
    auto clientY = YunoEngine::GetWindow()->GetClientHeight();

    CreateWidget<CardConfirmButton>(L"tstButton", XMFLOAT3(0, 0, 0), Float2(140, 55), UIDirection::LeftTop);
    CreateWidget<CardSlot>(L"tstSlot", XMFLOAT3(600, 100, 0), Float2(100, 135), UIDirection::LeftTop);
    CreateWidget<Card>(L"tstCard", XMFLOAT3(300, 100, 0), Float2(100, 135), UIDirection::LeftTop);
    

    //CreateWidget<CardConfirmButton>(L"tstButton", XMFLOAT3(0, 0, 0), Float2(140, 55), UIDirection::LeftTop);

    CreateWidget<CardConfirmPanel>(L"CardConformPanel", XMFLOAT3(0, clientY, 0), Float2(140, 55), UIDirection::LeftBottom);
    CreateWidget<CardSelectionPanel>(L"CardSelectionPanel", XMFLOAT3(clientX, clientY, 0), Float2(140, 55), UIDirection::RightBottom);


    //CreateWidget<CardTable>(L"tstCardTable", XMFLOAT3(500, 500, 0));
    //m_uiManager->CreateWidget<CardTable>(L"tstCardTable", XMFLOAT3(500, 500, 0)); 


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

void PhaseScene::OnDestroyScene()
{
    //std::cout << "[UIScene] OnDestroy\n";

}

void PhaseScene::TestInput()
{
}


void PhaseScene::OnEnter()
{
    //std::cout << "[UIScene] OnEnter\n"; 
    // 컨텍스트 추가
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void PhaseScene::OnExit()
{
    //std::cout << "[UIScene] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void PhaseScene::Update(float dt)
{
    //m_uiManager->GetCursurStstem()->UpdateCheckSnap();
    SceneBase::Update(dt); // 여기만 UI 출력하게끔 빼둘까?
    TestInput();
    //m_input->Dispatch();
    //m_minimap->Update(dt);
}

void PhaseScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void PhaseScene::SubmitUI()
{
    SceneBase::SubmitUI();
}

