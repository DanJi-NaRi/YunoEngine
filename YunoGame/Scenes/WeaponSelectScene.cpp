#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "WeaponSelectScene.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
//#include "ObjectManager.h"
#include "UIManager.h"
#include "CursurSystem.h"
#include "iwindow.h"

#include "GameManager.h"

// 여러 오브젝트들 ;; 
//#include "Building.h"
#include "UIWidgets.h"
#include "ShowCardDeck.h"

bool WeaponSelectScene::OnCreateScene()
{
    auto iwindow = YunoEngine::GetWindow();
    float ClientW = static_cast<float>(iwindow->GetClientWidth());
    float ClientH = static_cast<float>(iwindow->GetClientHeight());

    m_uiManager->SetOrthoFlag(true);
    m_pCursurSys = m_uiManager->GetCursurStstem();


    //CreateWidget<TitleImage>(L"Title", g_defWidgetSize, XMFLOAT3(0, 0, 0), XMFLOAT3(0.5f, 0.5f, 0.5f),UIDirection::LeftTop);
    CreateWidget<TextureImage>(L"BackGround", L"../Assets/UI/WEAPON_SELECT/background_weaponselect.png", XMFLOAT3(0, 0, 0));

    CreateWidget<TextureImage>(L"P1", L"../Assets/UI/WEAPON_SELECT/Weaponselect_background_1player.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"P2", L"../Assets/UI/WEAPON_SELECT/Weaponselect_background_2player.png", XMFLOAT3(0, 0, 0));

    CreateWidget<TextureImage>(L"WeaponSelect", L"../Assets/UI/WEAPON_SELECT/Weaponselect_background.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"CardBackground", L"../Assets/UI/WEAPON_SELECT/card_background.png", XMFLOAT3(0, 0, 0));



    m_pWeaponNameImage = CreateWidget<TextureImage>(L"WeaponName", L"../Assets/UI/WEAPON_SELECT/Weapon_name_blaster.png", XMFLOAT3(ClientW / 2, 255, 0), UIDirection::Center);
    m_pWeaponCardImage = CreateWidget<TextureImage>(L"WeaponCard", L"../Assets/UI/CARD/Card_back.png", XMFLOAT3(ClientW / 2, 400, 0), UIDirection::Center);
    
    m_showCardDeck = std::make_unique<ShowCardDeck>();
    m_showCardDeck->Create(*this, ClientW / 2.f, 390.f);
    m_showCardDeck->SetWeaponCards(PieceType::Blaster);


    m_pUserImage0 = CreateWidget<TextureImage>(L"S1", L"../Assets/UI/WEAPON_SELECT/Before_select_1.png", XMFLOAT3(0, 200, 0));
    m_pUserImage1 = CreateWidget<TextureImage>(L"S2", L"../Assets/UI/WEAPON_SELECT/Before_select_2.png", XMFLOAT3(0, 600, 0));
    m_pUserImage2 = CreateWidget<TextureImage>(L"S3", L"../Assets/UI/WEAPON_SELECT/Before_select_3.png", XMFLOAT3(ClientW, 200, 0), UIDirection::RightTop);
    m_pUserImage3 = CreateWidget<TextureImage>(L"S4", L"../Assets/UI/WEAPON_SELECT/Before_select_4.png", XMFLOAT3(ClientW, 600, 0), UIDirection::RightTop);

    m_pWeaponImage0 = CreateWidget<TextureImage>(L"W1", L"../Assets/UI/WEAPON_SELECT/weapon_1_blaster.png", XMFLOAT3(0, 200, 0));
    m_pWeaponImage1 = CreateWidget<TextureImage>(L"W2", L"../Assets/UI/WEAPON_SELECT/weapon_2_blaster.png", XMFLOAT3(0, 600, 0));
    m_pWeaponImage2 = CreateWidget<TextureImage>(L"W3", L"../Assets/UI/WEAPON_SELECT/weapon_3_blaster.png", XMFLOAT3(ClientW, 200, 0), UIDirection::RightTop);
    m_pWeaponImage3 = CreateWidget<TextureImage>(L"W4", L"../Assets/UI/WEAPON_SELECT/weapon_4_blaster.png", XMFLOAT3(ClientW, 600, 0), UIDirection::RightTop);

    m_pWeaponImage0->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
    m_pWeaponImage1->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
    m_pWeaponImage2->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
    m_pWeaponImage3->SetScale(XMFLOAT3(0.f, 0.f, 1.f));



    m_PWeaponBtn_Blaster = CreateWidget<WeaponButton>(L"W1Btn", Float2(287, 490), XMFLOAT3(574, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_BLASTER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_BLASTER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetPieceType(PieceType::Blaster);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetShowCardDeck(m_showCardDeck.get());



    m_PWeaponBtn_Chakram = CreateWidget<WeaponButton>(L"W2Btn", Float2(287, 490), XMFLOAT3(717, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_CHAKRAM_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_CHAKRAM_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetPieceType(PieceType::Chakram);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetShowCardDeck(m_showCardDeck.get());



    m_PWeaponBtn_Breacher = CreateWidget<WeaponButton>(L"W3Btn", Float2(287, 490), XMFLOAT3(860, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_BREACHER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_BREACHER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetPieceType(PieceType::Breacher);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetShowCardDeck(m_showCardDeck.get());
        


    m_PWeaponBtn_Scythe = CreateWidget<WeaponButton>(L"W4Btn", Float2(287, 490), XMFLOAT3(1003, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_SCYTHE_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_SCYTHE_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetPieceType(PieceType::Scythe);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetShowCardDeck(m_showCardDeck.get());



    m_PWeaponBtn_Impactor = CreateWidget<WeaponButton>(L"W5Btn", Float2(287, 490), XMFLOAT3(1146, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_IMPACTOR_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_IMPACTOR_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetPieceType(PieceType::Impactor);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetShowCardDeck(m_showCardDeck.get());



    m_PWeaponBtn_Cleaver = CreateWidget<WeaponButton>(L"W6Btn", Float2(287, 490), XMFLOAT3(1289, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Info_CLEAVER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_CLEAVER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetPieceType(PieceType::Cleaver);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetShowCardDeck(m_showCardDeck.get());



    m_pReadyBtn = CreateWidget<ReadyButton>(L"ReadyButton", Float2(487, 94), XMFLOAT3(1290, 1010, 0),UIDirection::Bottom);
    m_pReadyBtn->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Go_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Go_mouseover.png");



    m_pExitBtn = CreateWidget<SceneChangeButton>(L"ExitButton", Float2(487, 94), XMFLOAT3(520, 1010, 0), UIDirection::Bottom);
    m_pExitBtn->SetCursurTexture(L"../Assets/UI/WEAPON_SELECT/Exit_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Exit_mouseover.png");
    m_pExitBtn->SetTargetScene(CurrentSceneState::Title);

    // 선택창 UI
    //{
    //    m_pUserImage0 = CreateWidget<UserImage>(L"P1 U1", Float2(50, 50), XMFLOAT3(0, 0, 0));
    //    m_pUserImage1 = CreateWidget<UserImage>(L"P1 U2", Float2(50, 50), XMFLOAT3(0, 150, 0));
    //    m_pUserImage2 = CreateWidget<UserImage>(L"P2 U1", Float2(50, 50), XMFLOAT3(1000, 0, 0));
    //    m_pUserImage3 = CreateWidget<UserImage>(L"P2 U2", Float2(50, 50), XMFLOAT3(1000, 150, 0));
    //
    //    // 그리드 기능 쓰기??
    //    m_PWeaponBtn_Blaster = CreateWidget<WeaponButton>(L"Weapon_Blaster", Float2(50, 50), XMFLOAT3(250, 100, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetPieceType(PieceType::Blaster);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_PWeaponBtn_Impactor = CreateWidget<WeaponButton>(L"Weapon_Impactor", Float2(50, 50), XMFLOAT3(750, 100, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetPieceType(PieceType::Impactor);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_PWeaponBtn_Breacher = CreateWidget<WeaponButton>(L"Weapon_Breacher", Float2(50, 50), XMFLOAT3(500, 100, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetPieceType(PieceType::Breacher);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_PWeaponBtn_Chakram = CreateWidget<WeaponButton>(L"Weapon_Chakra", Float2(50, 50), XMFLOAT3(250, 300, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetPieceType(PieceType::Chakram);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_PWeaponBtn_Scythe = CreateWidget<WeaponButton>(L"Weapon_Scythe", Float2(50, 50), XMFLOAT3(500, 300, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetPieceType(PieceType::Scythe);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_PWeaponBtn_Cleaver = CreateWidget<WeaponButton>(L"Weapon_Cleaver", Float2(50, 50), XMFLOAT3(750, 300, 0));
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetPieceType(PieceType::Cleaver);
    //    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    //
    //    m_pReadyBtn = CreateWidget<ReadyButton>(L"ReadyButton", Float2(50, 50), XMFLOAT3(450, 500, 0));
    //    m_pExitBtn = CreateWidget<SceneChangeButton>(L"ExitButton", Float2(50, 50), XMFLOAT3(750, 500, 0));
    //    m_pExitBtn->SetTargetScene(CurrentSceneState::Title);
    //
    //
    //}

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
