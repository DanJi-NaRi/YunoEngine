#include "pch.h"

#include "TitleScene.h"

#include "YunoEngine.h"
#include "YunoCamera.h"

#include "ObjectManager.h"
//#include "Game_InputContext.h"
#include "IInput.h"

#include "Quad.h"
#include "Building.h"
#include "Triangle.h"
#include "Image.h"
#include "AnimTest.h"
#include "YunoLight.h"
#include "AudioQueue.h"

bool TitleScene::OnCreateScene()
{
    //std::cout << "[TitleScene] OnCreate\n";
    
    m_name = "TitleScene";
   
    m_objectManager->CreateDirLight();
    
    m_objectManager->CreateObject<Quad>(L"TitlePlane", XMFLOAT3(0, 0, 0));
    

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");

    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Building/building.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"LaserGun", XMFLOAT3(0, 2, 0), L"../Assets/fbx/LaserGun/LaserGun.fbx");
    auto capo = m_objectManager->CreateObjectFromFile<AnimTest>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Human/Capoeira2.fbx");
    capo->AddAnimationClip("capoeira", L"../Assets/fbx/Human/Capoeira2.fbx");
    //capo->SetScale(XMFLOAT3(10, 10, 10));
    capo->AddAnimationClip("dying", L"../Assets/fbx/Human/Dying2.fbx");
    //m_objectManager->CreateObjectFromFile<Building>(L"Buliding", XMFLOAT3(0, 0, 0), L"../Assets/fbx/Dwarf/Dwarf.fbx");


    return true;
}

void TitleScene::OnDestroyScene()
{   

}


void TitleScene::OnEnter()
{
    //std::cout << "[TitleScene] OnEnter\n";



    AudioQ::Insert(AudioQ::LoadBank(BankName::Title));
    AudioQ::Insert(AudioQ::PlayEvent(EventName::BGM_Playlist));

    YunoEngine::GetInput()->AddContext(&m_gameCtx, this);

}

void TitleScene::OnExit()
{
    //std::cout << "[TitleScene] OnExit\n";

    AudioQ::Insert(AudioQ::UnLoadBank(BankName::Title));
    //m_audioScene->Unload();
    YunoEngine::GetInput()->RemoveContext(&m_gameCtx);

}



void TitleScene::Update(float dt)
{
    SceneBase::Update(dt);
    m_input->Dispatch();
}

void TitleScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void TitleScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
