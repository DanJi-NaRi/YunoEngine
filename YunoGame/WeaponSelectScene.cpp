#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "WeaponSelectScene.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"

// 여러 오브젝트들 ;; 
#include "Building.h"


bool WeaponSelectScene::OnCreateScene()
{
    //std::cout << "[WeaponSelectScene] OnCreate\n";

    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    // 직교투영 필요한 씬만 ㄱㄱ
    //m_objectManager->SetOrthoFlag(true);


    m_objectManager->CreateObjectFromFile<Building>(L"Drill", XMFLOAT3(4, 2, 0), L"../Assets/fbx/Drill/Drill.fbx");

    return true;
}

void WeaponSelectScene::OnDestroyScene()
{
    //std::cout << "[WeaponSelectScene] OnDestroy\n";

}

void WeaponSelectScene::OnEnter()
{
    //std::cout << "[WeaponSelectScene] OnEnter\n"; 
}

void WeaponSelectScene::OnExit()
{
    //std::cout << "[WeaponSelectScene] OnExit\n"; 
}

void WeaponSelectScene::Update(float dt)
{
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);
}

void WeaponSelectScene::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void WeaponSelectScene::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
