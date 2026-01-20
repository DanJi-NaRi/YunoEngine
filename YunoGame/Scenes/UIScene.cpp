#include "pch.h"

#include "UIScene.h"

#include "YunoEngine.h"
#include "YunoCamera.h"

#include "ObjectManager.h"
#include "YunoLight.h"

#include "Image.h"
#include "Button.h"
#include "Building.h"
#include "Triangle.h"
#include "Quad.h"
#include "Dwarf.h"







bool UIScene::OnCreateScene()
{

    //std::cout << "[UIScene] OnCreate\n";
    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();
    m_objectManager->SetOrthoFlag(true);  



    m_objectManager->CreateWidget<Image>(L"tst", XMFLOAT3(0, 0, 0));
    m_objectManager->CreateWidget<Button>(L"tst", XMFLOAT3(50, 0, 0));

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");
    

    //m_objectManager->CreateWidget<Image>(L"tst", XMFLOAT3(0, 0, 0));


    return true;
}

void UIScene::OnDestroyScene()
{
    //std::cout << "[UIScene] OnDestroy\n";

}

void UIScene::OnEnter()
{
    //std::cout << "[UIScene] OnEnter\n"; 
}

void UIScene::OnExit()
{
    //std::cout << "[UIScene] OnExit\n"; 
}



void UIScene::Update(float dt)
{
    SceneBase::Update(dt); // 여기만 UI 출력하게끔 빼둘까?
}

void UIScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void UIScene::SubmitUI()
{
    SceneBase::SubmitUI();
}

