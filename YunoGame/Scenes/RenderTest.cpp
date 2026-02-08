#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "RenderTest.h"
// 다음 엔진
#include "YunoEngine.h"
#include "YunoCamera.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"
// 게임 매니저
#include "GameManager.h"
#include "UIManager.h"
#include "EffectManager.h"
#include "IInput.h"
// 여러 오브젝트들 ;; 
#include "Quad.h"
#include "Building.h"
#include "Triangle.h"
#include "Image.h"
#include "AnimTest.h"
#include "YunoLight.h"
#include "AudioQueue.h"
#include "PlayQueue.h"
#include "Text.h"
#include "SpriteSheet.h"
#include "EffectUnit.h"


// 사용법
// 컨트롤 + H 누르면 이름 변경 나옴
// 위쪽 칸에 SceneTemplate 적고
// 아래 칸에 원하는 씬 이름 적고
// 모두 바꾸기 누르면 됨 .h 파일도 동일하게 ㄱㄱ

bool RenderTest::OnCreateScene()
{
    YunoEngine::GetRenderer()->GetCamera().position = { 0, 5, -10 };
    YunoEngine::GetRenderer()->GetCamera().target = { 0, 0, -2 };
    m_uiManager->SetOrthoFlag(true);

    std::cout << "[RenderTest] OnCreate\n";

    m_name = "RenderTest";

    m_objectManager->CreateDirLight();

    int j = 0;
    for (int i = 0; i < 10; i++)
    {
        m_objectManager->CreatePointLight(XMFLOAT3(0, 1, 0), XMFLOAT4(1, 1, 1, 1), 30.0f);
        j++;
    }

    for (int i = -3; i <= 3; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            if (i == -3 || i == 3)
            {
                int abs = 1;
                if (i == 3)
                    abs = -1;

                XMFLOAT3 pos{ i * 1.07f, 0, j * abs * 1.19f };
                auto tile = m_objectManager->CreateObjectFromFile<AnimTest>(L"tile", pos, L"../Assets/fbx/Tile/floor1.fbx");
                tile->SetRot({ 0, XMConvertToRadians(90), 0 });

                tile->AddAnimationClip("idle", L"../Assets/fbx/Tile/floor1.fbx");
                tile->AddAnimationClip("fall", L"../Assets/fbx/Tile/Tile_anim/Floor1_crash_Anim.fbx");
            }
            else
            {
                XMFLOAT3 pos{ i * 1.07f, 0, j * 1.19f };
                auto tile = m_objectManager->CreateObjectFromFile<Building>(L"tile", pos, L"../Assets/fbx/Tile/floor6.fbx");
                tile->SetRot({ 0, XMConvertToRadians(90), 0 });
            }
        }
    }

    auto sheet = CreateWidget<SpriteSheet>(L"sheet", Float2{ 500, 500 }, XMFLOAT3{ 100, 100, 0 }, UIDirection::Center);
    sheet->SetSpriteSheet(L"../Assets/Effects/Main/EF_Main.png", 10, 6, 60, 24.f, true);

    PassOption po;

    auto map = m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Map/Mainmap.fbx", po);
    map->SetRot(XMFLOAT3(0, XMConvertToRadians(90), 0));
    //map->SetScale(XMFLOAT3(1, 1, 1));
    //m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Scythe/Scythe.fbx");
    //Building* tile;
    po.shader = ShaderId::PBR_AniDissolve;
    gun = m_objectManager->CreateObjectFromFile<AnimTest>(L"blaster" , XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Blaster/Blaster.fbx", po);
    gun->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/blaster_idle.fbx");
    gun->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Blaster_attack.fbx");
    gun->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
    gun->SetDissolveColor(XMFLOAT3(1, 1, 1));

    axe = m_objectManager->CreateObjectFromFile<AnimTest>(L"axe", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Impactor/Impactor.fbx");
    axe->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/impactor_idle.fbx");
    axe->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Impactor_attack2.fbx");
    //axe->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Untitled.fbx");

    chakram01 = m_objectManager->CreateObjectFromFile<AnimTest>(L"chakram01", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Chakram/Chakram01.fbx");
    chakram01->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/chakram_01_idle.fbx");
    chakram01->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Chakram_attack_01.fbx");

    chakram02 = m_objectManager->CreateObjectFromFile<AnimTest>(L"chakram02", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Chakram/Chakram02.fbx");
    chakram02->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/chakram_02_idle.fbx");
    chakram02->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Chakram_attack_02.fbx");

    blade = m_objectManager->CreateObjectFromFile<AnimTest>(L"blade", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Cleaver/Cleaver.fbx");
    blade->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/Cleaver_idle.fbx");
    blade->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Cleaver_attack.fbx");

    breacher = m_objectManager->CreateObjectFromFile<AnimTest>(L"breacher", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Breacher/Breacher.fbx");
    breacher->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/Breacher_idle.fbx");
    breacher->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Breacher_attack.fbx");
    //breacher->SetScale({ 2, 2, 2 });

    scythe = m_objectManager->CreateObjectFromFile<AnimTest>(L"scythe", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Scythe/Scythe.fbx");
    scythe->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/scythe_idle.fbx");
    scythe->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/scythe_attack.fbx");

    

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
    auto Holo = m_objectManager->CreateObjectFromFile<Building>(L"Holo_01", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Map/H/HOLD.fbx", po);
    auto Holo2 = m_objectManager->CreateObjectFromFile<Building>(L"Holo_02", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Map/H_2/HOLD.fbx", po);

    for (int i = 0; i < 6; i++)
    {
        auto razer = m_objectManager->CreateObjectFromFile<Building>(L"razer", XMFLOAT3(0, 1, 0), L"../Assets/fbx/Razer/Razer.fbx");
    }

    ed.framecount = 30;
    ed.cols = 5;
    ed.rows = 6;
    ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_2.png";

    EffectDesc ed2{};
    ed2.shaderid = ShaderId::EffectBase;
    ed2.billboard = BillboardMode::None;
    ed2.lifetime = 1.5f;
    ed2.framecount = 25;
    ed2.cols = 12;
    ed2.rows = 10;
    ed2.emissive = 30.0f;
    ed2.color = { 1, 0, 0, 1 };
    ed2.rot = { XM_PIDIV2, 0, 0 };
    ed2.isLoop = true;
    ed2.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_1.png";
    for (auto i = 0; i < 5; i++)
    {
        auto warning = m_objectManager->CreateObject<EffectUnit>(L"Warning_Floor1", XMFLOAT3(0, 1, 0));
        warning->BuildInternalEffectMaterial(ed);
        //warning = m_objectManager->CreateObject<EffectUnit>(L"Warning_Floor2", XMFLOAT3(0, 1, 0));
        //warning->BuildInternalEffectMaterial(ed2);
    }
   
    /////////////////////////////////////////////////////////////////////////////////
    // 이펙트 등록
    /////////////////////////////////////////////////////////////////////////////////
    ed.id = EffectID::PeacePosAlly;
    ed.shaderid = ShaderId::EffectBase;
    ed.billboard = BillboardMode::None;
    ed.lifetime = 1.5f;
    ed.framecount = 60;
    ed.cols = 8;
    ed.rows = 8;
    ed.emissive = 30.0f;
    ed.color = { 0, 0, 1, 1 };
    ed.rot = { XM_PIDIV2, 0, 0 };
    ed.isLoop = true;
    ed.texPath = L"../Assets/Effects/Pos/EF_Player_Blue.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::PeacePosEnemy;
    ed.color = { 1, 0, 0, 1 };
    ed.texPath = L"../Assets/Effects/Pos/EF_Player_Red.png";
    m_effectManager->RegisterEffect(ed);

    auto eff = m_effectManager->Spawn(EffectID::PeacePosAlly, { 0.0f, 0.01f, 0.f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    gun->Attach(eff);
    eff = m_effectManager->Spawn(EffectID::PeacePosAlly, { 0.0f, 0.01f, 0.0f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    axe->Attach(eff);
    eff = m_effectManager->Spawn(EffectID::PeacePosAlly, { 0.0f, 0.01f, 0.0f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    chakram01->Attach(eff);
    eff = m_effectManager->Spawn(EffectID::PeacePosEnemy, { 0.0f, 0.01f, 0.0f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    blade->Attach(eff);
    eff = m_effectManager->Spawn(EffectID::PeacePosEnemy, { 0.0f, 0.01f, 0.0f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    breacher->Attach(eff);
    eff = m_effectManager->Spawn(EffectID::PeacePosEnemy, { 0.0f, 0.01f, 0.0f }, { 1.f, 1.f, 1.f }, { 1, 0, 0 });
    scythe->Attach(eff);

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
    ed.billboard = BillboardMode::Beam;
    ed.texPath = L"../Assets/Effects/Blaster/EF_Shot.png";
    ed.emissive = 50.0f;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::AxAttack;
    ed.lifetime = 0.8f;
    ed.framecount = 12;
    ed.cols = 4;
    ed.rows = 3;
    ed.billboard = BillboardMode::None;
    ed.texPath = L"../Assets/Effects/Ax/EF_LongSword.png";
    ed.emissive = 50.0f;
    ed.color = { 1, 0, 0, 1 };
    ed.rot = { XM_PIDIV2, 0, 0 };
    m_effectManager->RegisterEffect(ed);

    ed.isLoop = true;
    ed.id = EffectID::DrillAttack1;
    ed.framecount = 30;
    ed.lifetime = 1.2f;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::Beam;
    ed.rot = { 0, 0, 0 };
    ed.texPath = L"../Assets/Effects/Drill/EF_Drill.png";
    m_effectManager->RegisterEffect(ed);

    ed.isLoop = false;
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

    ed.id = EffectID::Hit;
    ed.framecount = 25;
    ed.lifetime = 1.2f;
    ed.cols = 5;
    ed.rows = 5;
    ed.billboard = BillboardMode::ScreenAligned;
    ed.rot = { 0, 0, 0 };
    ed.emissive = 10.0f;
    ed.color = { 1, 0, 0, 1 };
    ed.texPath = L"../Assets/Effects/Hit/EF_Hits.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Target;
    ed.framecount = 30;
    ed.lifetime = 1.2f;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.rot = { XM_PIDIV2, 0, 0 };
    ed.emissive = 30.0f;
    ed.color = { 0, 0, 1, 1 };
    ed.texPath = L"../Assets/Effects/Target/EF_Target.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Target;
    ed.framecount = 30;
    ed.lifetime = 1.2f;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::None;
    ed.rot = { XM_PIDIV2, 0, 0 };
    ed.emissive = 30.0f;
    ed.color = { 0, 0, 1, 1 };
    ed.texPath = L"../Assets/Effects/Target/EF_Target.png";
    m_effectManager->RegisterEffect(ed);

    return true;
}

void RenderTest::OnDestroyScene()
{
    //std::cout << "[SceneTemplate] OnDestroy\n";

}

void RenderTest::OnEnter()
{
    //std::cout << "[SceneTemplate] OnEnter\n"; 
}

void RenderTest::OnExit()
{
    //std::cout << "[SceneTemplate] OnExit\n"; 
}

void RenderTest::Update(float dt)
{
    if (YunoEngine::GetInput()->IsKeyPressed('1'))
    {
        auto eff = m_effectManager->Spawn(EffectID::DrillAttack1, { 0.1f, 0.25f, 0.1f }, { 1.f, 1.f, 1.f }, {1, 0, 0});
        breacher->AttachChildBone(eff, 1);
    }
    if (YunoEngine::GetInput()->IsKeyPressed('2'))
    {
        auto eff = m_effectManager->Spawn(EffectID::Buff, { 0.1f, 0.4f, 0 }, { 1.f, 1.f, 1.f });
        breacher->Attach(eff);
    }
    if (YunoEngine::GetInput()->IsKeyPressed('3'))
    {
        auto eff = m_effectManager->Spawn(EffectID::Hit, { 0.15f, 0.5f, 0 }, { 1.5f, 1.5f, 1.5f });
        breacher->Attach(eff);
    }
    if (YunoEngine::GetInput()->IsKeyPressed('4'))
    {
        auto eff = m_effectManager->Spawn(EffectID::Target, { -2.14f, 0.05f, 1.19f }, { 1.f, 1.f, 1.f });
        eff = m_effectManager->Spawn(EffectID::Target, { -1.07f, 0.05f, 1.19f }, { 1.f, 1.f, 1.f });
        eff = m_effectManager->Spawn(EffectID::Target, { 0.0f, 0.05f, 1.19f }, { 1.f, 1.f, 1.f });
        eff = m_effectManager->Spawn(EffectID::Target, { 1.07f, 0.05f, 1.19f }, { 1.f, 1.f, 1.f });
        eff = m_effectManager->Spawn(EffectID::Target, { 2.14f, 0.05f, 1.19f }, { 1.f, 1.f, 1.f });
    }
    if (YunoEngine::GetInput()->IsKeyPressed('F'))
    {
        auto eff = m_effectManager->Spawn(EffectID::Razer, { 0.f, 0.8f, 2.38f }, { 11.f, 1.f, 1.f }, { 0, 1, 0 });
        eff = m_effectManager->Spawn(EffectID::Razer, { 0.f, 0.8f, 0.f }, { 11.f, 1.f, 1.f }, { -1, 0, 0 });
        eff = m_effectManager->Spawn(EffectID::Razer, { 0.f, 0.8f, -2.38f }, { 11.f, 1.f, 1.f }, { 0, 1, 0 });
    }
    if (YunoEngine::GetInput()->IsKeyPressed('G'))
    {
        auto eff = m_effectManager->Spawn(EffectID::BlasterAttack, { 0.f, 0.f, 0.15f }, { 1.f, 1.f, 1.f }, { -1, 0, 0 });
        gun->AttachChildBone(eff, 3);
    }
    if (YunoEngine::GetInput()->IsKeyPressed('H'))
    {
        auto eff = m_effectManager->Spawn(EffectID::AxAttack, { -0.15f, 0.1f, 0.f }, { 1.f, 1.f, 1.f });
        axe->Attach(eff);
    }
    
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);
}

void RenderTest::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void RenderTest::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
