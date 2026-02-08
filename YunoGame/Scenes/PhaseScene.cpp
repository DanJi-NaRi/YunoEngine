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

    m_uiManager->SetOrthoFlag(true);


    auto clientX = YunoEngine::GetWindow()->GetClientWidth();
    auto clientY = YunoEngine::GetWindow()->GetClientHeight();

    //CreateWidget<CardConfirmButton>(L"tstButton", XMFLOAT3(0, 0, 0), Float2(140, 55), UIDirection::LeftTop);
    //CreateWidget<CardSlot>(L"tstSlot", XMFLOAT3(600, 100, 0), Float2(100, 135), UIDirection::LeftTop);
    CreateWidget<Card>(L"tstCard", Float2(100, 135), XMFLOAT3(300, 100, 0), XMFLOAT3(0.5f, 0.5f, 0.5f), UIDirection::LeftTop);

    
    {
        CreateWidget<CardConfirmPanel>(L"CardConformPanel", Float2(700, 500), XMFLOAT3(0, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftBottom);
        CreateWidget<Minimap>(L"Minimap", Float2(500, 500), XMFLOAT3(clientX * 0.5f, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::Bottom);
        CreateWidget<CardSelectionPanel>(L"CardSelectionPanel", Float2(700, 600), XMFLOAT3(clientX, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::RightBottom);
    }
    //CreateWidget<CardConfirmPanel>(L"CardConformPanel", Float2(1566, 578), XMFLOAT3(0, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftBottom);
    
    //CreateWidget<Minimap>(L"Minimap", Float2(730, 616), XMFLOAT3(clientX*0.5f, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::Bottom);

    //테스트용
    //<WidgetGridLine>(L"_DebugWidgetGridLine", Float2(90, 90), XMFLOAT3(clientX*0.5f, clientY * 0.5f, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::Center);
    //CreateWidget<MinimapTile>(L"_tstTile", Float2(90, 90), XMFLOAT3(clientX * 0.5f, clientY * 0.5f, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::Center);
    //std::cout << clientX << " : " << clientY << std::endl;
    //CreateWidget<CardSelectionPanel>(L"CardSelectionPanel", Float2(1513, 578), XMFLOAT3(clientX, clientY, 0), XMFLOAT3(1, 1, 1),  UIDirection::RightBottom);

    

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



    auto* a = CreateWidget<Letterbox>(L"LetterBoxA", Float2(50, 50), XMFLOAT3(0, 0, 0) );
    a->SetRole(Letterbox::Role::BarA);
    auto* b = CreateWidget<Letterbox>(L"LetterBoxB", Float2(50, 50), XMFLOAT3(0, 0, 0));
    b->SetRole(Letterbox::Role::BarB);
    //m_minimap = std::make_unique<MinimapGridSystem>(m_uiManager.get(), m_input);
    //m_minimap->CreateObject(1000, 900, 0);

//m_uiManager->AllParentsSetScale(0.5f); // 일괄 사이즈 조정

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

