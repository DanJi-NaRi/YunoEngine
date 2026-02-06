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

#include "PlayQueue.h"






bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    m_name = "PlayScene";

    // 플레이 그리드 시스템 생성
    m_playGrid = std::make_unique<PlayGridSystem>(m_objectManager.get());
    m_playGrid->CreateObject(0, 1, 0);
    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();


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
void PlayScene::HandleCardSelect(int key, int unit, int index)
{
    if (!m_input->IsKeyPressed(key))
        return;

    if (m_pendingCardRuntimeID != 0)
        return; // 이미 카드 하나 대기 중이면 무시

    int unitSlot = m_selectedUnitSlot;  // 0, 1 첫번째 무기인지 두번째 무기인지

    uint32_t runtimeID =
        GameManager::Get().GetMyCardRuntimeID(unit, index);

    if (runtimeID == 0)
        return;

    m_pendingCardRuntimeID = runtimeID;

    std::cout << "[Card Selected] unitSlot="
        << (unitSlot + 1)
        << " index=" << index
        << " runtimeID=" << runtimeID
        << " (waiting for direction)\n";
}

void PlayScene::HandleDirectionInput()
{
    if (m_pendingCardRuntimeID == 0)
        return;

    Direction dir = Direction::None;

    if (m_input->IsKeyPressed(VK_UP))    dir = Direction::Up;
    if (m_input->IsKeyPressed(VK_DOWN))  dir = Direction::Down;
    if (m_input->IsKeyPressed(VK_LEFT))  dir = Direction::Left;
    if (m_input->IsKeyPressed(VK_RIGHT)) dir = Direction::Right;

    if (dir == Direction::None)
        return;

    CardPlayCommand cmd;
    cmd.runtimeID = m_pendingCardRuntimeID;
    cmd.dir = dir;

    if (GameManager::Get().PushCardCommand(cmd))
    {
        std::cout << "[Card Queued] runtimeID="
            << cmd.runtimeID
            << " dir=" << static_cast<int>(cmd.dir)
            << "\n";

        m_pendingCardRuntimeID = 0; // 대기 해제
    }
    else
    {
        std::cout << "[Card Queue] Full\n";
    }
}

void PlayScene::AddCardSelect()
{
    int index = -1;

    if (m_input->IsKeyPressed('Z'))
        index = 0;
    else if (m_input->IsKeyPressed('X'))
        index = 1;
    else if (m_input->IsKeyPressed('C'))
        index = 2;

    if (index == -1)
        return;

    const auto& candidates =
        GameManager::Get().GetDrawCandidates();

    if (index >= candidates.size())
        return;

    uint32_t runtimeID = candidates[index].runtimeID;

    GameManager::Get().SendSelectCard(index);

    std::cout << "[Client] Select card index="
        << index
        << " runtimeID="
        << runtimeID
        << "\n";
}
//TODO: 디버깅용 나중에는 UI씬에 OnEndTurnClicked함수 만들어서 옮겨야함
void PlayScene::EndTurn()
{
    if (GameManager::Get().IsCardQueueEmpty())
        return;

    GameManager::Get().SubmitTurn(
        GameManager::Get().GetCardQueue()
    );
    // TODO:
    // - 입력 잠금
    // - 서버에서 턴 결과 올 때까지 대기
    // - 결과 수신 시 큐 Clear
}


void PlayScene::TestInput()
{
    AddCardSelect();
    // 디버깅용
    // 카드 선택 (넘버패드 = UI 버튼 대용)
    HandleCardSelect(VK_NUMPAD1, 0, 0); // 클라 1
    HandleCardSelect(VK_NUMPAD2, 0, 2);
    HandleCardSelect(VK_NUMPAD3, 0, 4);
    HandleCardSelect(VK_NUMPAD4, 0, 15);

    HandleCardSelect(VK_NUMPAD5, 1, 0); // 클라 2
    HandleCardSelect(VK_NUMPAD6, 1, 2);
    HandleCardSelect(VK_NUMPAD7, 1, 4);
    HandleCardSelect(VK_NUMPAD8, 1, 15);

    HandleCardSelect(VK_NUMPAD9, 1,8);

    //방향 선택
    HandleDirectionInput();
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
