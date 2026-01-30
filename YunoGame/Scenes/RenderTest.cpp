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
// 여러 오브젝트들 ;; 
#include "Quad.h"
#include "Building.h"
#include "Triangle.h"
#include "Image.h"
#include "AnimTest.h"
#include "YunoLight.h"
#include "AudioQueue.h"
#include "PlayQueue.h"


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
    m_objectManager->CreatePointLight(XMFLOAT3(0, 1, 0), XMFLOAT4(1, 1, 1, 1), 50.0f);
    //m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));


    //m_gridSystem = std::make_unique<PlayGridSystem>(m_objectManager.get());
    //m_gridSystem->Init(5, 7, 2, 2);
    //m_gridSystem->CreateObject(0, 1, 0);


    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    auto map = m_objectManager->CreateObjectFromFile<Building>(L"Map", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Map/background.fbx");
    map->SetRot(XMFLOAT3(0, XMConvertToRadians(90), 0));
    map->SetScale(XMFLOAT3(1, 1, 1));

    auto gun = m_objectManager->CreateObjectFromFile<Building>(L"LaserGun", XMFLOAT3(0, 2, 0), L"../Assets/fbx/LaserGun/LaserGun.fbx");
    gun->SetRot(XMFLOAT3(XMConvertToRadians(-24), XMConvertToRadians(-90), 0));

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
