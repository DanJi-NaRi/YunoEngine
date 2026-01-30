#include "pch.h"

#include "PlayScene.h"

#include "YunoEngine.h"

#include "ObjectManager.h"
#include "GameManager.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "IInput.h"
//#include "Game_InputContext.h"

#include "PlayGridSystem.h"
#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"

#include "PieceQueue.h"






bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    m_name = "PlayScene";

    // 플레이 그리드 시스템 생성
    m_playGrid = std::make_unique<PlayGridSystem>(m_objectManager.get());
    m_playGrid->Init(5, 7, 3, 3);
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

//TODO:  UIScene에 OnCardButtonClicked함수로 옮겨야 함
void PlayScene::HandleCardSelect(int key, int index)
{
    if (!m_input->IsKeyPressed(key))
        return;
    
    uint32_t runtimeID =
        GameManager::Get().GetTestCardRuntimeIDByIndex(index);

    if (runtimeID == 0)
        return;

    if (m_cardQueue.Push(runtimeID))
    {
        std::cout << "[Card Select] runtimeID = "
            << runtimeID << "\n";
    }
    else
    {
        std::cout << "[Card Select] Queue Full\n";
    }
}

//TODO: 디버깅용 나중에는 UI씬에 OnEndTurnClicked함수 만들어서 옮겨야함
void PlayScene::EndTurn()
{
    if (m_cardQueue.IsEmpty())
        return;

    GameManager::Get().SubmitTurn(m_cardQueue.Get());
    // TODO:
    // - 입력 잠금
    // - 서버에서 턴 결과 올 때까지 대기
    // - 결과 수신 시 큐 Clear
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

    // 디버깅용
    // 카드 선택 (넘버패드 = UI 버튼 대용)
    HandleCardSelect(VK_NUMPAD1, 0);
    HandleCardSelect(VK_NUMPAD2, 1);
    HandleCardSelect(VK_NUMPAD3, 2);
    HandleCardSelect(VK_NUMPAD4, 3);
    HandleCardSelect(VK_NUMPAD5, 4);
    HandleCardSelect(VK_NUMPAD6, 5);
    HandleCardSelect(VK_NUMPAD7, 6);
    HandleCardSelect(VK_NUMPAD8, 7);
    HandleCardSelect(VK_NUMPAD9, 8);

    // 엔드 턴
    if (m_input->IsKeyPressed(VK_RETURN))
    {
        EndTurn();
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
