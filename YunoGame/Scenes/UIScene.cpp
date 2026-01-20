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
    YunoEngine::GetRenderer()->GetCamera().SetOrthoFlag(true);
    //std::cout << "[UIScene] OnCreate\n";

    // 있는지 체크
    ObjectManager* om = GetObjectManager();
    if (!om) return false;
    
    // 디렉션 라이트 생성
    m_objectManager->CreateDirLight();

    //om->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));
    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");
    //om->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    om->CreateWidget<Image>(L"tst", XMFLOAT3(0, 0, 0));
    om->CreateWidget<Button>(L"tst", XMFLOAT3(50, 0, 0));
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

void UIScene::Submit()
{
    SceneBase::Submit();
}
