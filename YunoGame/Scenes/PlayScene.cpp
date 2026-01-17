#include "pch.h"

#include "PlayScene.h"

#include "YunoEngine.h"

#include "ObjectManager.h"
#include "YunoLight.h"


#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"






bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    // 있는지 체크
    ObjectManager* om = GetObjectManager();
    if (!om) return false;
    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();

    //om->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));


    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");
    om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    // 플레이어(삼각형)
    //om->CreateObject<Triangle>(L"player", XMFLOAT3(0, 0, 0));
    return true;
}

void PlayScene::OnDestroyScene()
{
    //std::cout << "[PlayScene] OnDestroy\n";

}

void PlayScene::OnEnter()
{
    //std::cout << "[PlayScene] OnEnter\n"; 
}

void PlayScene::OnExit()
{
    //std::cout << "[PlayScene] OnExit\n"; 
}

void PlayScene::Update(float dt)
{


    SceneBase::Update(dt);
}

void PlayScene::Submit()
{
    SceneBase::Submit();
}
