#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "RenderTest.h"
// 다음 엔진
#include "YunoEngine.h"
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


// 사용법
// 컨트롤 + H 누르면 이름 변경 나옴
// 위쪽 칸에 SceneTemplate 적고
// 아래 칸에 원하는 씬 이름 적고
// 모두 바꾸기 누르면 됨 .h 파일도 동일하게 ㄱㄱ

bool RenderTest::OnCreateScene()
{
    std::cout << "[RenderTest] OnCreate\n";

    m_name = "RenderTest";

    m_objectManager->CreateDirLight();

    int j = 0;
    for (int i = -3; i < 3; i++)
    {
        m_objectManager->CreatePointLight(XMFLOAT3(i * 2, 1, 0), XMFLOAT4(1, 1, 1, 1), 30.0f);
        j++;
    }
    //m_objectManager->CreatePointLight(XMFLOAT3(0, 1, 0), XMFLOAT4(1, 1, 1, 1), 50.0f);
    //m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));


    //m_gridSystem = std::make_unique<PlayGridSystem>(m_objectManager.get());
    //m_gridSystem->Init(5, 7, 2, 2);
    //m_gridSystem->CreateObject(0, 1, 0);

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    PassOption po;

    auto map = m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Map/Mainmap.fbx", po);
    map->SetRot(XMFLOAT3(0, XMConvertToRadians(90), 0));
    //map->SetScale(XMFLOAT3(1, 1, 1));
    
    //Building* tile;
    //tile = m_objectManager->CreateObjectFromFile<Building>(L"Tile" , XMFLOAT3(0, 0, 0), L"../Assets/fbx/Tile/floor1.fbx");

    //gun = m_objectManager->CreateObjectFromFile<Building>(L"LaserGun", XMFLOAT3(0, 2, 0), L"../Assets/fbx/LaserGun/LaserGun.fbx", po);

    for(int i = 0; i < 30; i++)
    {
    }
    gun = m_objectManager->CreateObjectFromFile<AnimTest>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Tile/floor1.fbx");

    gun->AddAnimationClip("capo1", L"../Assets/fbx/Tile/floor1.fbx");
    gun->AddAnimationClip("capo2", L"../Assets/fbx/Tile/Tile_anim/Floor1_crash_Anim.fbx");
    //gun->AddAnimationClip("capo2", L"../Assets/fbx/Tile/Tile_anim/Floor2_crash_Anim.fbx");
    //gun->AddAnimationClip("capo3", L"../Assets/fbx/Tile/Tile_anim/Floor3_crash_Anim.fbx");
    //gun->AddAnimationClip("capo4", L"../Assets/fbx/Tile/Tile_anim/Floor4_crash_Anim.fbx");
    //gun->AddAnimationClip("capo5", L"../Assets/fbx/Tile/Tile_anim/Floor5_crash_Anim.fbx");
    //gun->AddAnimationClip("capo6", L"../Assets/fbx/Tile/Tile_anim/Floor1_Wave_Anim.fbx");
    //gun->AddAnimationClip("capo7", L"../Assets/fbx/Tile/Tile_anim/Floor2_Wave_Anim.fbx");
    //gun->AddAnimationClip("capo8", L"../Assets/fbx/Tile/Tile_anim/Floor3_Wave_Anim.fbx");
    //gun->AddAnimationClip("capo9", L"../Assets/fbx/Tile/Tile_anim/Floor4_Wave_Anim.fbx");
    //gun->AddAnimationClip("capo10", L"../Assets/fbx/Tile/Tile_anim/Floor5_Wave_Anim.fbx");

    //gun->SetScale({ 100, 100, 100 });
    //gun->SetScale({ 0.01f, 0.01f, 0.01f });

    //auto text = m_uiManager->CreateWidget<Text>(L"TestText", XMFLOAT3(1, 1, 0));
    //text->SetText(L"Hello World!");
    //gun->SetRot(XMFLOAT3(XMConvertToRadians(-24), XMConvertToRadians(-90), 0));

    //auto gun2 = m_objectManager->CreateObjectFromFile<Building>(L"LaserGun2", XMFLOAT3(0, 2, 2), L"../Assets/fbx/LaserGun/LaserGun.fbx", po);
    //gun2->SetRot(XMFLOAT3(XMConvertToRadians(-24), XMConvertToRadians(-90), 0));

    EffectDesc ed{};
    ed.id = EffectID::Razer;
    ed.shaderid = ShaderId::EffectBase;
    ed.billboard = BillboardMode::ScreenAligned;
    ed.lifetime = 1.0f;
    ed.framecount = 100;
    ed.cols = 6;
    ed.rows = 5;
    ed.texPath = L"../Assets/Effects/Razer/EF_Razer_Yellow.png";
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::HitRed;
    ed.framecount = 30;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::AxisLocked;
    ed.texPath = L"../Assets/Effects/Razer/EF_Rager.png";
    ed.emissive = 1.0f;
    ed.color = { 1, 0, 0, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::HitBlue;
    ed.color = { 0, 0, 1, 1 };
    m_effectManager->RegisterEffect(ed);

    ed.id = EffectID::Default;
    ed.framecount = 30;
    ed.cols = 6;
    ed.rows = 5;
    ed.billboard = BillboardMode::ScreenAligned;
    ed.texPath = L"../Assets/Effects/Lazer/EF_Rager.png";
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
    if (YunoEngine::GetInput()->IsKeyPressed('F'))
    {
        auto eff = m_effectManager->Spawn(EffectID::HitBlue, { 1, 0.1f, 1.f }, { 1.5f, 1.5f, 1.5f });
        gun->Attach(eff);
    }
    if (YunoEngine::GetInput()->IsKeyPressed('G'))
    {
        auto eff = m_effectManager->Spawn(EffectID::Razer, { 0.f, 2.f, 0.f }, { 1.5f, 5.f, 1.5f });
    }
    if (YunoEngine::GetInput()->IsKeyPressed('H'))
    {
        auto eff = m_effectManager->Spawn(EffectID::HitRed, { 0, 0.5f, 0.f }, { 1.5f, 1.5f, 1.5f }, {-1, 0, 0});
        gun->AttachChildBone(eff, 1);
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
