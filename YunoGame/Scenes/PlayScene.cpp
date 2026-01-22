#include "pch.h"

#include "PlayScene.h"

#include "YunoEngine.h"

#include "ObjectManager.h"
#include "YunoLight.h"
#include "YunoCamera.h"

#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"






bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    m_name = "PlayScene";

    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();


    m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));



    m_objectManager->CreateObjectFromFile<Dwarf>(L"Dwarf", XMFLOAT3(-5, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");

    m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    // 플레이어(삼각형)
    m_objectManager->CreateObject<Triangle>(L"player", XMFLOAT3(5, 0, 0));


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

void PlayScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void PlayScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
