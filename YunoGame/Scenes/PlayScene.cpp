#include "pch.h"

#include "PlayScene.h"

#include "YunoEngine.h"

#include "ObjectManager.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "IInput.h"
//#include "Game_InputContext.h"

#include "PlayGridSystem.h"
#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"

#include "PlayQueue.h"






bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    m_name = "PlayScene";

    // 플레이 그리드 시스템 생성
    m_playGrid = std::make_unique<PlayGridSystem>(m_objectManager.get());
    m_playGrid->CreateObject(0, 1, 0);
    
    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();


    //m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));



    //m_objectManager->CreateObjectFromFile<Dwarf>(L"Dwarf", XMFLOAT3(-5, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    // 플레이어(삼각형)
    //m_objectManager->CreateObject<Triangle>(L"player", XMFLOAT3(5, 0, 0));


    return true;
}

void PlayScene::OnDestroyScene()
{
    //std::cout << "[PlayScene] OnDestroy\n";

}

void PlayScene::TestInput()
{
    // 테스트용 -> ally1으로 부여한 기물이 움직여용
    if (m_input->IsKeyPressed(0x31))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 0, 3));   // 왼쪽
    }
    if (m_input->IsKeyPressed(0x32))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 1, 2));   // 아래
    }
    if (m_input->IsKeyPressed(0x33))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 2, 3));   // 위
    }
    if (m_input->IsKeyPressed(0x34))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 1, 4));   // 오른쪽
    }
    if (m_input->IsKeyPressed(0x35))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 0, 2));   // 왼쪽 위
    }
    if (m_input->IsKeyPressed(0x36))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 2, 2));   // 오른쪽 위
    }
    if (m_input->IsKeyPressed(0x37))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 0, 4));   // 왼쪽 아래
    }
    if (m_input->IsKeyPressed(0x38))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 2, 4));   // 오른쪽 아래
    }
    if (m_input->IsKeyPressed(0x39))
    {
        PlayGridQ::Insert(PlayGridQ::Attack_S_TST(GamePiece::Ally1));   // 공격
    }
    if (m_input->IsKeyPressed(0x30))
    {
        PlayGridQ::Insert(PlayGridQ::Move_S(GamePiece::Ally1, 1, 3));   // 충돌
    }
}


void PlayScene::OnEnter()
{
    //std::cout << "[PlayScene] OnEnter\n"; 
    YunoEngine::GetInput()->AddContext(&m_gameCtx, this);
}

void PlayScene::OnExit()
{
    //std::cout << "[PlayScene] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_gameCtx);
}

void PlayScene::Update(float dt)
{
    SceneBase::Update(dt);
    //m_input->Dispatch();
    TestInput();

    m_playGrid->Update(dt);

}

void PlayScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void PlayScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
