#include "pch.h"

#include "PlayScene.h"

#include "YunoEngine.h"

#include "ObjectManager.h"
#include "GameManager.h"
#include "EffectManager.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "IInput.h"
#include "AudioQueue.h"
//#include "Game_InputContext.h"

#include "PlayGridSystem.h"
#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"
#include "Emoji.h"
#include "PlayQueue.h"
#include "EffectUnit.h"


bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    m_name = "PlayScene";


    m_uiManager->SetOrthoFlag(true);//여기서 ui 쓰는거 맞음?
    m_objectManager->CreateDirLight();

    int j = 0;
    for (int i = 0; i < 12; i++)
    {
        m_objectManager->CreatePointLight(XMFLOAT3(0, 1, 0), XMFLOAT4(1, 1, 1, 1), 30.0f);
        j++;
    }

    PassOption po;
    po.shader = ShaderId::NoneShadowPBRBase;

    auto map = m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Map/Mainmap.fbx", po);
    map->SetRot({ 0, XM_PIDIV2, 0 });

    EffectDesc ed{};
    ed.id = EffectID::Warning;
    ed.shaderid = ShaderId::EffectBase;
    ed.billboard = BillboardMode::None;
    ed.lifetime = 1.5f;
    ed.framecount = 25;
    ed.cols = 5;
    ed.rows = 5;
    ed.emissive = 30.0f;
    ed.color = { 1, 0, 0, 1 };
    ed.rot = { 0, 0, 0 };
    ed.isLoop = true;
    ed.texPath = L"../Assets/Effects/Warning/EF_WARNING.png";

    for (auto i = 0; i < 5; i++)
    {
        auto warning = m_objectManager->CreateObject<EffectUnit>(L"Warning", XMFLOAT3(0, 1, 0));
        warning->BuildInternalEffectMaterial(ed);
    }

    ed.framecount = 25;
    ed.cols = 5;
    ed.rows = 5;
    ed.texPath = L"../Assets/Effects/Warning/EF_WARNING_2.png";
    for (auto i = 0; i < 5; i++)
    {
        auto warning = m_objectManager->CreateObject<EffectUnit>(L"Warning_2", XMFLOAT3(0, 1, 0));
        warning->BuildInternalEffectMaterial(ed);
    }

    po = {};
    po.raster = RasterPreset::CullNone;
    po.blend = BlendPreset::AlphaBlend;
    po.shader = ShaderId::EffectWithTexMap;

    for (auto i = 0; i < 5; i++)
    {
        auto Holo = m_objectManager->CreateObjectFromFile<Building>(L"Holo_01_", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Map/H/HOLD.fbx", po);
        Holo->SetRot({ -XM_PIDIV2, 0, 0 });
        auto Holo2 = m_objectManager->CreateObjectFromFile<Building>(L"Holo_02_", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Map/H_2/HOLD.fbx", po);
        Holo2->SetRot({ -XM_PIDIV2, 0, 0 });
    }

    for (auto i = 0; i < 10; i++)
    {
        auto Holo3 = m_objectManager->CreateObjectFromFile<Building>(L"Holo_03_", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Map/H_3/VS.fbx", po);
    }

    for (int i = 0; i < 6; i++)
    {
        auto razer = m_objectManager->CreateObjectFromFile<Building>(L"razer", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Razer/Razer.fbx");
    }

    // 플레이 그리드 시스템 생성
    m_playGrid = std::make_unique<PlayGridSystem>(m_objectManager.get(), m_effectManager.get());
    m_playGrid->CreateObject(0, 0, 0);
    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();

    RegisterEffect();

    // 오디오 추가
    AudioQ::Insert(AudioQ::StopOrRestartEvent(EventName::BGM_Lobby, true));
    AudioQ::Insert(AudioQ::PlayEvent(EventName::BGM_Main));

    return true;
}

void PlayScene::RegisterEffect()
{
    EffectDesc ed{};

    ed.id = EffectID::Razer;
    ed.shaderid = ShaderId::EffectBase;
    ed.billboard = BillboardMode::Beam;
    ed.lifetime = 1.1f;
    ed.framecount = 30;
    ed.cols = 6;
    ed.rows = 5;
    ed.emissive = 50.0f;
    ed.color = { 1, 0, 0, 1 };
    ed.rot = { 0, 0, 0 };
    ed.isLoop = false;
    ed.texPath = L"../Assets/Effects/Razer/EF_Rager_White.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::BlasterAttack;
    ed.lifetime = 1.2f;
    ed.framecount = 30;
    ed.cols = 5;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.texPath = L"../Assets/Effects/Blaster/EF_Shot.png";
    ed.emissive = 50.0f;
    ed.color = { 0, 0.5f, 1, 1 };
    ed.rot = { 0, -XM_PIDIV2, -XM_PIDIV2 };
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::BlasterAttackEnemy;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::AxAttack;
    ed.lifetime = 0.4f;
    ed.framecount = 12;
    ed.cols = 4;
    ed.rows = 3;
    ed.billboard = BillboardMode::None;
    ed.texPath = L"../Assets/Effects/Ax/EF_LongSword.png";
    ed.emissive = 50.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.rot = { XM_PIDIV2, 0, 0 };
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::AxAttackEnemy;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::AxAttack2;
    ed.lifetime = 0.5f;
    ed.framecount = 30;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.texPath = L"../Assets/Effects/Ax/EF_Fragment.png";
    ed.emissive = 50.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.rot = { 0, XM_PIDIV2, 0 };
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::AxAttackEnemy2;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::DrillAttack1;
    ed.framecount = 30;
    ed.lifetime = 1.2f;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.rot = { 0, -XM_PIDIV2, -XM_PIDIV2 };
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Drill/EF_Drill.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::DrillAttackEnemy1;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Buff;
    ed.framecount = 25;
    ed.lifetime = 1.2f;
    ed.cols = 5;
    ed.rows = 5;
    ed.billboard = BillboardMode::ScreenAligned;
    ed.rot = { 0, 0, 0 };
    ed.emissive = 50.0f;
    ed.color = { 0, 1, 0, 1 };
    ed.texPath = L"../Assets/Effects/Buff/EF_Buff.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::StaminaBuff;
    ed.color = { 0, 1, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::PowerUpBuff;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::ShieldBuff;
    ed.color = { 1, 1, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::DeBuff;
    ed.color = { 0, 0, 1, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Hit;
    ed.framecount = 25;
    ed.lifetime = 1.2f;
    ed.cols = 5;
    ed.rows = 5;
    ed.billboard = BillboardMode::ScreenAligned;
    ed.rot = { 0, 0, 0 };
    ed.emissive = 10.0f;
    ed.color = { 1, 0.5f, 0, 1 };
    ed.texPath = L"../Assets/Effects/Hit/EF_Hits.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Target;
    ed.framecount = 25;
    ed.lifetime = 0.9f;
    ed.cols = 5;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.rot = { XM_PIDIV2, 0, 0 };
    ed.emissive = 30.0f;
    ed.color = { XMFLOAT4{ 0, 0.1f, 1, 1 } };
    ed.texPath = L"../Assets/Effects/Target/EF_Target.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::TargetEnemy;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::ScytheAttack;
    ed.framecount = 18;
    ed.lifetime = 0.4f;
    ed.cols = 3;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { 0, XM_PIDIV2, XMConvertToRadians(275) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_1.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ScytheAttackEnemy;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::ScytheAttack2;
    ed.framecount = 30;
    ed.lifetime = 0.4f;
    ed.cols = 5;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { 0, XM_PIDIV2, -XMConvertToRadians(275) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_2.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ScytheAttackEnemy2;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    //1번 차크람
    ed.id = EffectID::ChakramAttack01_1;
    ed.framecount = 18;
    ed.lifetime = 0.4f;
    ed.cols = 3;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { -XM_PIDIV2, XMConvertToRadians(120), XMConvertToRadians(40) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_1.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ChakramAttackEnemy01_1;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::ChakramAttack01_2;
    ed.framecount = 30;
    ed.lifetime = 0.4f;
    ed.cols = 5;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { XM_PIDIV2, XMConvertToRadians(120), XMConvertToRadians(150) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_2.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ChakramAttackEnemy01_2;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    //2번 차크람
    ed.id = EffectID::ChakramAttack02_1;
    ed.framecount = 18;
    ed.lifetime = 0.4f;
    ed.cols = 3;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { XM_PIDIV2, XMConvertToRadians(120), XMConvertToRadians(70) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_1.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ChakramAttackEnemy02_1;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::ChakramAttack02_2;
    ed.framecount = 30;
    ed.lifetime = 0.4f;
    ed.cols = 5;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { -XM_PIDIV2, XMConvertToRadians(120), XMConvertToRadians(110) };
    ed.emissive = 30.0f;
    ed.color = { 0, 1, 1, 1 };
    ed.texPath = L"../Assets/Effects/Scythe/EF_Scythe_2.png";
    m_effectManager->RegisterEffect(ed);
    ed.id = EffectID::ChakramAttackEnemy02_2;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::FloorWarning1;
    ed.framecount = 120;
    ed.lifetime = 0.4f;
    ed.cols = 12;
    ed.rows = 10;
    ed.billboard = BillboardMode::None;
    ed.rot = { XMConvertToRadians(90), 0, 0 };
    ed.emissive = 30.0f;
    ed.color = { 1, 1, 0, 1 };
    ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_1.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::FloorWarning2;
    ed.framecount = 30;
    ed.lifetime = 0.4f;
    ed.cols = 5;
    ed.rows = 6;
    ed.billboard = BillboardMode::None;
    ed.rot = { XMConvertToRadians(90), 0, 0 };
    ed.emissive = 30.0f;
    ed.color = { 1, 1, 0, 1 };
    ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_2.png";
    m_effectManager->RegisterEffect(ed);
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
        GameManager::Get().GetMyCardRuntimeID(unit, index-1);

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

    //if (m_input->IsKeyPressed('Z'))
    //    index = 0;
    //else if (m_input->IsKeyPressed('X'))
    //    index = 1;
    //else if (m_input->IsKeyPressed('C'))
    //    index = 2;

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
    //AddCardSelect();
    // 디버깅용
    // 카드 선택 (넘버패드 = UI 버튼 대용)
    HandleCardSelect(VK_NUMPAD1, 0, 1); //버프
    HandleCardSelect(VK_NUMPAD2, 0, 3); //이동
    HandleCardSelect(VK_NUMPAD3, 0, 5); //공격
    HandleCardSelect(VK_NUMPAD4, 0, 12);//특수

    HandleCardSelect(VK_NUMPAD5, 1, 1); //버프
    HandleCardSelect(VK_NUMPAD6, 1, 3); //이동
    HandleCardSelect(VK_NUMPAD7, 1, 5); //공격
    HandleCardSelect(VK_NUMPAD8, 1, 12);//특수

    HandleCardSelect(VK_NUMPAD9, 1,9);

   

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

    m_playGrid->ApplyTransform();

    m_CurSceneState = GameManager::Get().GetSceneState();
    m_PrevSceneState = m_CurSceneState;

    
    m_CurCamPos = { 0.0f, 5.672f, -3.07f };
    m_CurCamTarget = { 0, 0, 0 };
    m_NextCamPos = { 0.0f, 4.043f, -5.651f };
    m_NextCamTarget = { 0.0f, 3.293f, -6.313f };
    isCamMove = true;
}

void PlayScene::OnExit()
{
    //std::cout << "[PlayScene] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_gameCtx);
}


void PlayScene::MoveCamera(float dt)
{
    if (!isCamMove)
        return;

    auto& cam = YunoEngine::GetRenderer()->GetCamera();

    m_curMoveTime += dt;

    float t = m_curMoveTime / m_CamMoveTime;

    XMVECTOR curpos = XMLoadFloat3(&m_CurCamPos);
    XMVECTOR curTarget = XMLoadFloat3(&m_CurCamTarget);
    XMVECTOR nextpos = XMLoadFloat3(&m_NextCamPos);
    XMVECTOR nextTarget = XMLoadFloat3(&m_NextCamTarget);

    XMVECTOR newpos;
    XMVECTOR newTarget;
    if (t < 1.0f)
    {
        newpos = XMVectorLerp(curpos, nextpos, t);
        newTarget = XMVectorLerp(curTarget, nextTarget, t);
        XMStoreFloat3(&cam.position, newpos);
        XMStoreFloat3(&cam.target, newTarget);
    }
    else
    {
        newpos = nextpos;
        newTarget = nextTarget;
        XMStoreFloat3(&cam.position, newpos);
        XMStoreFloat3(&cam.target, newTarget);

        XMStoreFloat3(&m_NextCamPos, curpos);
        XMStoreFloat3(&m_NextCamTarget, curTarget);
        XMStoreFloat3(&m_CurCamPos, newpos);
        XMStoreFloat3(&m_CurCamTarget, newTarget);

        m_curMoveTime = 0.0f;
        isCamMove = false;
    }
}


void PlayScene::Update(float dt)
{
    SceneBase::Update(dt);
    //m_input->Dispatch();
    //TestInput();

    m_playGrid->Update(dt);

    m_CurSceneState = GameManager::Get().GetSceneState();

    if (m_PrevSceneState != m_CurSceneState)
    {
        if (m_CurSceneState == CurrentSceneState::SubmitCard
            || m_CurSceneState == CurrentSceneState::AutoBattle)
            isCamMove = true;
    }

    MoveCamera(dt);

    if (m_input->IsKeyPressed(VK_ESCAPE))
    {
        GameManager::Get().SetSceneState(CurrentSceneState::EscScene);
    }

    m_PrevSceneState = m_CurSceneState;
}

void PlayScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void PlayScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
