#include "pch.h"

#include "PlayScene.h"

#include "Triangle.h"


#include "YunoEngine.h"
#include "IInput.h"
#include "ISceneManager.h"

#include "TitleScene.h"
#include "ObjectManager.h"

bool PlayScene::OnCreateScene()
{
    //std::cout << "[PlayScene] OnCreate\n";

    // 있는지 체크
    ObjectManager* om = GetObjectManager();
    if (!om) return false;

    // 플레이어(삼각형)
    m_player = om->CreateObject<Triangle>(L"player", XMFLOAT3(0, 0, 0));
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
