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
#include "AudioQueue.h"


#include "MinimapGridSystem.h"



bool PhaseScene::OnCreateScene()
{

    m_uiManager->SetOrthoFlag(true);


    //auto clientX = YunoEngine::GetWindow()->GetClientWidth();
    //auto clientY = YunoEngine::GetWindow()->GetClientHeight();

    const auto clientX = g_DefaultClientXY.x;
    const auto clientY = g_DefaultClientXY.y;

    //CreateWidget<CardConfirmButton>(L"tstButton", XMFLOAT3(0, 0, 0), Float2(140, 55), UIDirection::LeftTop);
    //CreateWidget<CardSlot>(L"tstSlot", XMFLOAT3(600, 100, 0), Float2(100, 135), UIDirection::LeftTop);


    
    // 테스트용 사이즈
    /*{
        CreateWidget<CardConfirmPanel>(L"CardConformPanel", Float2(700, 500), XMFLOAT3(0, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftBottom);
        CreateWidget<Minimap>(L"Minimap", Float2(500, 500), XMFLOAT3(clientX * 0.5f, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::Bottom);
        CreateWidget<CardSelectionPanel>(L"CardSelectionPanel", Float2(700, 600), XMFLOAT3(clientX, clientY, 0), XMFLOAT3(1, 1, 1), UIDirection::RightBottom);
    }*/

    auto& gm = GameManager::Get();


    // 원본 사이즈 
    m_pMinimap = CreateWidget<Minimap>(L"Minimap", Float2(730, 616), XMFLOAT3(851, 1072, 0), XMFLOAT3(1, 1, 1), UIDirection::Bottom);
    gm.SetMinimap(m_pMinimap);
    m_pMinimap->SetupPanel();
   
    m_pSelectionPanel = CreateWidget<CardSelectionPanel>(L"CardSelectionPanel", Float2(1513, 578), XMFLOAT3(1825, 1052, 0), XMFLOAT3(1, 1, 1), UIDirection::RightBottom);
    gm.SetSelectionPanel(m_pSelectionPanel);

    m_pConfirmPanel = CreateWidget<CardConfirmPanel>(L"CardConformPanel", Float2(1066, 579), XMFLOAT3(99, 1054, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftBottom);        // 이거 사이즈 달라서 고쳐둠
    gm.SetConfirmPanel(m_pConfirmPanel);
    
    m_pMinimap->BindCardConfirmPanel(m_pConfirmPanel);
    m_pConfirmPanel->BindMinimap(m_pMinimap); // 미니맵 할당 순서 주의!!
    m_pConfirmPanel->BindSelectionPanel(m_pSelectionPanel);
    


    //테스트용
    //<WidgetGridLine>(L"_DebugWidgetGridLine", Float2(90, 90), XMFLOAT3(clientX*0.5f, clientY * 0.5f, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::Center);
    //CreateWidget<MinimapTile>(L"_tstTile", Float2(90, 90), XMFLOAT3(clientX * 0.5f, clientY * 0.5f, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::Center);
    //std::cout << clientX << " : " << clientY << std::endl;
    

    auto* a = CreateWidget<Letterbox>(L"LetterBoxA", Float2(50, 50), XMFLOAT3(0, 0, 0) );
    a->SetRole(Letterbox::Role::BarA);
    auto* b = CreateWidget<Letterbox>(L"LetterBoxB", Float2(50, 50), XMFLOAT3(0, 0, 0));
    b->SetRole(Letterbox::Role::BarB);

    //m_minimap = std::make_unique<MinimapGridSystem>(m_uiManager.get(), m_input);

    m_uiManager->AllParentsSetScale(0.5f); // 일괄 사이즈 조정

    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_Move));

    return true;
}

void PhaseScene::OnDestroyScene()
{
    std::cout << "===================" << std::endl;
    std::cout << "[UIScene] OnDestroy\n";
    std::cout << "===================" << std::endl;

    if (!GameManager::HasInstance())
    {
        return;
    }

    auto& gm = GameManager::Get();
    gm.SetMinimap(nullptr);
    gm.SetSelectionPanel(nullptr);
    gm.SetConfirmPanel(nullptr);
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

