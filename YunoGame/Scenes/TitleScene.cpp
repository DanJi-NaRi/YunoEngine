#include "pch.h"

#include "TitleScene.h"

#include "YunoEngine.h"
#include "YunoCamera.h"

#include "ObjectManager.h"

#include "Quad.h"
#include "Building.h"
#include "Triangle.h"
#include "Image.h"
#include "AnimTest.h"
#include "YunoLight.h"


bool TitleScene::OnCreateScene()
{
    //std::cout << "[TitleScene] OnCreate\n";
    
    m_name = "TitleScene";
   
    m_objectManager->CreateDirLight();
    
    m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));
    

    m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building_TextureOn.fbx");
    m_objectManager->CreateObjectFromFile<AnimTest>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Human/human2.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");


    return true;
}

void TitleScene::OnDestroyScene()
{   

}

TitleScene::TitleScene()
{
    m_id = SceneID::Title;
}

void TitleScene::OnEnter()
{
    //std::cout << "[TitleScene] OnEnter\n";
    m_AudioManager->Load("Title");
    m_AudioManager->PlayEvent("BGM/Playlist");
}

void TitleScene::OnExit()
{
    //std::cout << "[TitleScene] OnExit\n";
    m_AudioManager->Unload();
}



void TitleScene::Update(float dt)
{
    SceneBase::Update(dt);
}

void TitleScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void TitleScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
