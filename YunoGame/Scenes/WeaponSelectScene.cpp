#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "WeaponSelectScene.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
//#include "ObjectManager.h"
#include "UIManager.h"
#include "CursurSystem.h"

#include "GameManager.h"

// 여러 오브젝트들 ;; 
//#include "Building.h"
#include "UIWidgets.h"


bool WeaponSelectScene::OnCreateScene()
{
    //std::cout << "[WeaponSelectScene] OnCreate\n";

    // 디렉션 라이트 생성
    //m_objectManager->CreateDirLight();
    // 직교투영 필요한 씬만 ㄱㄱ
    m_uiManager->SetOrthoFlag(true);
    m_pCursurSys = m_uiManager->GetCursurStstem();


    // 선택창 UI
    {
        m_pUserImage0 = m_uiManager->CreateWidget<UserImage>(L"P1 U1", XMFLOAT3(0, 0, 0));
        m_pUserImage1 = m_uiManager->CreateWidget<UserImage>(L"P1 U2", XMFLOAT3(0, 150, 0));
        m_pUserImage2 = m_uiManager->CreateWidget<UserImage>(L"P2 U1", XMFLOAT3(1000, 0, 0));
        m_pUserImage3 = m_uiManager->CreateWidget<UserImage>(L"P2 U2", XMFLOAT3(1000, 150, 0));

        // 그리드 기능 쓰기??
        m_PWeaponBtn_Blaster    = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Blaster",  XMFLOAT3(250, 100, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetPieceType(PieceType::Blaster);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_PWeaponBtn_Impactor   = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Impactor", XMFLOAT3(750, 100, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetPieceType(PieceType::Impactor);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_PWeaponBtn_Breacher   = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Breacher", XMFLOAT3(500, 100, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetPieceType(PieceType::Breacher);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_PWeaponBtn_Chakram    = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Chakra",   XMFLOAT3(250, 300, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetPieceType(PieceType::Chakram);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_PWeaponBtn_Scythe     = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Scythe",   XMFLOAT3(500, 300, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetPieceType(PieceType::Scythe);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_PWeaponBtn_Cleaver    = m_uiManager->CreateWidget<WeaponButton>(L"Weapon_Cleaver",  XMFLOAT3(750, 300, 0));
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetPieceType(PieceType::Cleaver);
        dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);

        m_pReadyBtn             = m_uiManager->CreateWidget<ReadyButton>(L"ReadyButton",      XMFLOAT3(450, 500, 0));
        m_pExitBtn              = m_uiManager->CreateWidget<ExitButton>(L"ExitButton",        XMFLOAT3(750, 500, 0));


    }

    //m_objectManager->CreateObjectFromFile<Building>(L"Drill", XMFLOAT3(4, 2, 0), L"../Assets/fbx/Drill/Drill.fbx");

    return true;
}

void WeaponSelectScene::OnDestroyScene()
{
    //std::cout << "[WeaponSelectScene] OnDestroy\n";

}

void WeaponSelectScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_weaponCtx, this); 
    //std::cout << "[WeaponSelectScene] OnEnter\n"; 
}

void WeaponSelectScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_weaponCtx);
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
