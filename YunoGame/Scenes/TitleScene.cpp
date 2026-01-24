#include "pch.h"

#include "TitleScene.h"

#include "YunoEngine.h"
#include "YunoCamera.h"

#include "ObjectManager.h"
//#include "Game_InputContext.h"
#include "IInput.h"

#include "PlayGridSystem.h"
#include "Quad.h"
#include "Building.h"
#include "Triangle.h"
#include "Image.h"
#include "AnimTest.h"
#include "YunoLight.h"
#include "AudioQueue.h"
#include "PieceQueue.h"

bool TitleScene::OnCreateScene()
{
    //std::cout << "[TitleScene] OnCreate\n";
    // 그리드 오브젝트 안에서 그리드 시스템을 만들어
    // 행렬, 사이즈.

    m_name = "TitleScene";
   
    m_objectManager->CreateDirLight();
    
    //m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));

    m_gridSystem = std::make_unique<PlayGridSystem>(m_objectManager.get());
    m_gridSystem->Init(5, 7, 3, 2);
    m_gridSystem->CreateObject(0, 10, 0);

    
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    //m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Map/background.fbx");

    auto gun = m_objectManager->CreateObjectFromFile<Building>(L"LaserGun", XMFLOAT3(0, 2, 0), L"../Assets/fbx/LaserGun/LaserGun.fbx");
    gun->SetRot(XMFLOAT3(XMConvertToRadians(-24), XMConvertToRadians(-90), 0));
    gun->SetEmissiveColor(0, XMFLOAT4(1, 0, 0, 1));
    m_objectManager->CreateObjectFromFile<Building>(L"Drill", XMFLOAT3(4, 2, 0), L"../Assets/fbx/Drill/Drill.fbx");
    m_objectManager->CreateObjectFromFile<Building>(L"Axe", XMFLOAT3(-2, 2, 0), L"../Assets/fbx/Ax/Ax.fbx");
    //auto capo = m_objectManager->CreateObjectFromFile<AnimTest>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Human/Capoeira2.fbx");
    //capo->AddAnimationClip("capoeira", L"../Assets/fbx/Human/Capoeira2.fbx");
   // capo->AddAnimationClip("dying", L"../Assets/fbx/Human/Dying2.fbx");


    return true;
}

void TitleScene::OnDestroyScene()
{   
   
}


TitleScene::TitleScene() = default;

TitleScene::~TitleScene() = default;

void TitleScene::OnEnter()
{
    //std::cout << "[TitleScene] OnEnter\n";

    AudioQ::Insert(AudioQ::LoadBank(BankName::Title));
    //AudioQ::Insert(AudioQ::PlayEvent(EventName::BGM_Playlist));

    YunoEngine::GetInput()->AddContext(&m_gameCtx, this);

    
}

void TitleScene::OnExit()
{
    //std::cout << "[TitleScene] OnExit\n";

    AudioQ::Insert(AudioQ::UnLoadBank(BankName::Title));
    //m_audioScene->Unload();
    YunoEngine::GetInput()->RemoveContext(&m_gameCtx);

}



void TitleScene::Update(float dt)
{
    SceneBase::Update(dt);

    if (m_input->IsKeyPressed(VK_OEM_PERIOD))
    {
        AudioQ::Insert(AudioQ::StopOrRestartEvent(EventName::BGM_Playlist, true));
        AudioQ::Insert(AudioQ::StopOrRestartEvent(EventName::BGM_Playlist, false));
    }

    // 테스트용 -> ally1으로 부여한 기물이 움직여용
    if (m_input->IsKeyPressed(0x31))
    {
        PieceQ::Insert({ PieceType::Ally1, CommandType::Move, {0, 0} });
    }
    if (m_input->IsKeyPressed(0x32))
    {
        PieceQ::Insert({ PieceType::Ally1, CommandType::Move, {1, 1} });
    }
    if (m_input->IsKeyPressed(0x33))
    {
        PieceQ::Insert({ PieceType::Ally1, CommandType::Move, {2, 2} });
    }

    m_input->Dispatch();
    m_gridSystem->Update(dt);
}

void TitleScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void TitleScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
