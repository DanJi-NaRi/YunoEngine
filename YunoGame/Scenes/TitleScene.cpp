#include "pch.h"

#include "TitleScene.h"

#include "Quad.h"
#include "Building.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "ISceneManager.h"

#include "PlayScene.h"
#include "ObjectManager.h"


bool TitleScene::OnCreateScene()
{
    //std::cout << "[TitleScene] OnCreate\n";

    // 있는지 체크
    ObjectManager* om = GetObjectManager();
    if (!om) return false;

    om->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));
    //
    om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

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

    // 이거만 있으면 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);

}

void TitleScene::Submit()
{
    // 이거만 있으면 됨 따로 서브밋 ㄴㄴ
    SceneBase::Submit();
}
