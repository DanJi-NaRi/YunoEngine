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
    YunoEngine::GetRenderer()->GetCamera().SetOrthoFlag(false);

    // 있는지 체크
    ObjectManager* om = GetObjectManager();
    if (!om) return false;
    om->CreateDirLight();
    

    
    om->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));
    
    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");
    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building_TextureOn.fbx");
    om->CreateObjectFromFile<AnimTest>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Human/human2.fbx");
    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");

    return true;
}

void TitleScene::OnDestroyScene()
{   

}

void TitleScene::OnEnter()
{
    //std::cout << "[TitleScene] OnEnter\n";
}

void TitleScene::OnExit()
{
    //std::cout << "[TitleScene] OnExit\n";
}

void TitleScene::Update(float dt)
{
    YunoEngine::GetRenderer()->GetCamera().SetOrthoFlag(false);
    SceneBase::Update(dt);

}

void TitleScene::Submit()
{
    SceneBase::Submit();
}
