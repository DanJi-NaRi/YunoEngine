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



namespace
{
    std::wstring GetPieceNameLower(PieceType pieceType)
    {
        switch (pieceType)
        {
        case PieceType::Blaster: return L"blaster";
        case PieceType::Chakram: return L"chakram";
        case PieceType::Breacher: return L"breacher";
        case PieceType::Scythe: return L"scythe";
        case PieceType::Impactor: return L"impactor";
        case PieceType::Cleaver: return L"cleaver";
        case PieceType::None:
        default: return L"";
        }
    }

    std::wstring GetSelectedSlotTexturePath(PieceType pieceType, int slotTextureIndex)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/selected_" + pieceName + L"_" + std::to_wstring(slotTextureIndex) + L".png";
    }

    std::wstring GetWeaponSlotTexturePath(PieceType pieceType, int slotTextureIndex)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/weapon_" + std::to_wstring(slotTextureIndex) + L"_" + pieceName + L".png";
    }

    std::wstring GetReadySlotTexturePath(int slotTextureIndex)
    {
        if (slotTextureIndex < 1 || slotTextureIndex > 4)
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/Ready_" + std::to_wstring(slotTextureIndex) + L".png";
    }
}

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



    m_pWeaponNameImage = CreateWidget<TextureImage>(L"WeaponName", L"../Assets/UI/WEAPON_SELECT/Weapon_name_select.png", XMFLOAT3(ClientW / 2, 255, 0), UIDirection::Center);
    //m_pWeaponCardImage = CreateWidget<TextureImage>(L"WeaponCard", L"../Assets/UI/CARD/Card_back.png", XMFLOAT3(ClientW / 2, 400, 0), UIDirection::Center);
    



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

    m_showCardDeck = CreateWidget<ShowCardDeck>(L"ShowCardDeck", Float2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));

    m_PWeaponBtn_Blaster = CreateWidget<WeaponButton>(L"W1Btn", Float2(287, 490), XMFLOAT3(574, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_BLASTER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_BLASTER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetPieceType(PieceType::Blaster);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Blaster)->SetShowCardDeck(m_showCardDeck);



    m_PWeaponBtn_Chakram = CreateWidget<WeaponButton>(L"W2Btn", Float2(287, 490), XMFLOAT3(717, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_CHAKRAM_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_CHAKRAM_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetPieceType(PieceType::Chakram);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Chakram)->SetShowCardDeck(m_showCardDeck);



    m_PWeaponBtn_Breacher = CreateWidget<WeaponButton>(L"W3Btn", Float2(287, 490), XMFLOAT3(860, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_BREACHER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_BREACHER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetPieceType(PieceType::Breacher);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Breacher)->SetShowCardDeck(m_showCardDeck);
        


    m_PWeaponBtn_Scythe = CreateWidget<WeaponButton>(L"W4Btn", Float2(287, 490), XMFLOAT3(1003, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_SCYTHE_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_SCYTHE_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetPieceType(PieceType::Scythe);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Scythe)->SetShowCardDeck(m_showCardDeck);



    m_PWeaponBtn_Impactor = CreateWidget<WeaponButton>(L"W5Btn", Float2(287, 490), XMFLOAT3(1146, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_IMPACTOR_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_IMPACTOR_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetPieceType(PieceType::Impactor);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Impactor)->SetShowCardDeck(m_showCardDeck);



    m_PWeaponBtn_Cleaver = CreateWidget<WeaponButton>(L"W6Btn", Float2(287, 490), XMFLOAT3(1289, 925, 0), UIDirection::Bottom);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Info_CLEAVER_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Info_CLEAVER_mouseover.png");
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetPieceType(PieceType::Cleaver);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetUserImages(m_pUserImage0, m_pUserImage1, m_pUserImage2, m_pUserImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetWeaponImages(m_pWeaponImage0, m_pWeaponImage1, m_pWeaponImage2, m_pWeaponImage3);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetWeaponPreviewImages(m_pWeaponNameImage, m_pWeaponCardImage);
    dynamic_cast<WeaponButton*>(m_PWeaponBtn_Cleaver)->SetShowCardDeck(m_showCardDeck);



    m_pReadyBtn = CreateWidget<ReadyButton>(L"ReadyButton", Float2(487, 94), XMFLOAT3(1290, 1010, 0),UIDirection::Bottom);
    m_pReadyBtn->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Go_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Go_mouseover.png");



    m_pExitBtn = CreateWidget<SceneChangeButton>(L"ExitButton", Float2(487, 94), XMFLOAT3(520, 1010, 0), UIDirection::Bottom);
    m_pExitBtn->SetHoverTexture(L"../Assets/UI/WEAPON_SELECT/Exit_mouseout.png", L"../Assets/UI/WEAPON_SELECT/Exit_mouseover.png");
    m_pExitBtn->SetTargetScene(CurrentSceneState::Title);





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

void WeaponSelectScene::ApplyReadyStateVisuals()
{
    GameManager& gm = GameManager::Get();

    const int mySlot = gm.GetSlotiIdx();
    if (mySlot != 1 && mySlot != 2)
        return;

    if (!gm.IsReady())
        return;

    TextureImage* myUserSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage0)
        : dynamic_cast<TextureImage*>(m_pUserImage2);
    TextureImage* myUserSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage1)
        : dynamic_cast<TextureImage*>(m_pUserImage3);

    if (!myUserSlot0 || !myUserSlot1)
        return;

    const int mySlotTextureBase = (mySlot == 1) ? 1 : 3;

    const std::wstring myReady0 = GetReadySlotTexturePath(mySlotTextureBase);
    const std::wstring myReady1 = GetReadySlotTexturePath(mySlotTextureBase + 1);

    if (!myReady0.empty()) myUserSlot0->ChangeTexture(myReady0);
    if (!myReady1.empty()) myUserSlot1->ChangeTexture(myReady1);
}

void WeaponSelectScene::ApplyOpponentPresenceVisuals()
{
    GameManager& gm = GameManager::Get();

    const int mySlot = gm.GetSlotiIdx();
    if (mySlot != 1 && mySlot != 2)
        return;

    TextureImage* enemyUserSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage2)
        : dynamic_cast<TextureImage*>(m_pUserImage0);
    TextureImage* enemyUserSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage3)
        : dynamic_cast<TextureImage*>(m_pUserImage1);

    if (!enemyUserSlot0 || !enemyUserSlot1)
        return;

    const int enemySlot = (mySlot == 1) ? 2 : 1;
    const int enemySlotTextureBase = (enemySlot == 1) ? 1 : 3;

    //const std::wstring opponentStateTexture0 = gm.HasOpponentInMatchRoom()
    //    ? L"../Assets/UI/WEAPON_SELECT/selecting_" + std::to_wstring(enemySlotTextureBase) + L".png"
    //    : L"../Assets/UI/WEAPON_SELECT/non_select_" + std::to_wstring(enemySlotTextureBase) + L".png";
    //const std::wstring opponentStateTexture1 = gm.HasOpponentInMatchRoom()
    //    ? L"../Assets/UI/WEAPON_SELECT/selecting_" + std::to_wstring(enemySlotTextureBase + 1) + L".png"
    //    : L"../Assets/UI/WEAPON_SELECT/non_select_" + std::to_wstring(enemySlotTextureBase + 1) + L".png";
    //
    //enemyUserSlot0->ChangeTexture(opponentStateTexture0);
    //enemyUserSlot1->ChangeTexture(opponentStateTexture1);

    const bool opponentInRoom = gm.HasOpponentInMatchRoom();
    const bool opponentReady = gm.IsOpponentReady();

    std::wstring opponentStateTexture0;
    std::wstring opponentStateTexture1;

    if (!opponentInRoom)
    {
        opponentStateTexture0 = L"../Assets/UI/WEAPON_SELECT/non_select_" + std::to_wstring(enemySlotTextureBase) + L".png";
        opponentStateTexture1 = L"../Assets/UI/WEAPON_SELECT/non_select_" + std::to_wstring(enemySlotTextureBase + 1) + L".png";
    }
    else if (opponentReady)
    {
        opponentStateTexture0 = GetReadySlotTexturePath(enemySlotTextureBase);
        opponentStateTexture1 = GetReadySlotTexturePath(enemySlotTextureBase + 1);
    }
    else
    {
        opponentStateTexture0 = L"../Assets/UI/WEAPON_SELECT/selecting_" + std::to_wstring(enemySlotTextureBase) + L".png";
        opponentStateTexture1 = L"../Assets/UI/WEAPON_SELECT/selecting_" + std::to_wstring(enemySlotTextureBase + 1) + L".png";
    }

    if (!opponentStateTexture0.empty()) enemyUserSlot0->ChangeTexture(opponentStateTexture0);
    if (!opponentStateTexture1.empty()) enemyUserSlot1->ChangeTexture(opponentStateTexture1);
}


void WeaponSelectScene::ApplyCountDownUnits()
{
    GameManager& gm = GameManager::Get();

    const int mySlot = gm.GetSlotiIdx();
    if (mySlot != 1 && mySlot != 2)
        return;

    TextureImage* myUserSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage0)
        : dynamic_cast<TextureImage*>(m_pUserImage2);
    TextureImage* myUserSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage1)
        : dynamic_cast<TextureImage*>(m_pUserImage3);

    TextureImage* myWeaponSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pWeaponImage0)
        : dynamic_cast<TextureImage*>(m_pWeaponImage2);
    TextureImage* myWeaponSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pWeaponImage1)
        : dynamic_cast<TextureImage*>(m_pWeaponImage3);

    TextureImage* enemyUserSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage2)
        : dynamic_cast<TextureImage*>(m_pUserImage0);
    TextureImage* enemyUserSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pUserImage3)
        : dynamic_cast<TextureImage*>(m_pUserImage1);

    TextureImage* enemyWeaponSlot0 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pWeaponImage2)
        : dynamic_cast<TextureImage*>(m_pWeaponImage0);
    TextureImage* enemyWeaponSlot1 = (mySlot == 1)
        ? dynamic_cast<TextureImage*>(m_pWeaponImage3)
        : dynamic_cast<TextureImage*>(m_pWeaponImage1);

    if (!myUserSlot0 || !myUserSlot1 || !myWeaponSlot0 || !myWeaponSlot1 ||
        !enemyUserSlot0 || !enemyUserSlot1 || !enemyWeaponSlot0 || !enemyWeaponSlot1)
    {
        return;
    }

    const int enemySlot = (mySlot == 1) ? 2 : 1;

    const PieceType myUnit0 = static_cast<PieceType>(gm.GetCountDownSlotUnitId(mySlot, 0));
    const PieceType myUnit1 = static_cast<PieceType>(gm.GetCountDownSlotUnitId(mySlot, 1));
    const PieceType enemyUnit0 = static_cast<PieceType>(gm.GetCountDownSlotUnitId(enemySlot, 0));
    const PieceType enemyUnit1 = static_cast<PieceType>(gm.GetCountDownSlotUnitId(enemySlot, 1));

    const int mySlotTextureBase = (mySlot == 1) ? 1 : 3;
    const int enemySlotTextureBase = (enemySlot == 1) ? 1 : 3;

    const std::wstring mySelected0 = GetSelectedSlotTexturePath(myUnit0, mySlotTextureBase);
    const std::wstring mySelected1 = GetSelectedSlotTexturePath(myUnit1, mySlotTextureBase + 1);
    const std::wstring myWeapon0 = GetWeaponSlotTexturePath(myUnit0, mySlotTextureBase);
    const std::wstring myWeapon1 = GetWeaponSlotTexturePath(myUnit1, mySlotTextureBase + 1);
    const std::wstring myReady0 = GetReadySlotTexturePath(mySlotTextureBase);
    const std::wstring myReady1 = GetReadySlotTexturePath(mySlotTextureBase + 1);



    const std::wstring enemySelected0 = GetSelectedSlotTexturePath(enemyUnit0, enemySlotTextureBase);
    const std::wstring enemySelected1 = GetSelectedSlotTexturePath(enemyUnit1, enemySlotTextureBase + 1);
    const std::wstring enemyWeapon0 = GetWeaponSlotTexturePath(enemyUnit0, enemySlotTextureBase);
    const std::wstring enemyWeapon1 = GetWeaponSlotTexturePath(enemyUnit1, enemySlotTextureBase + 1);
    const std::wstring enemyReady0 = GetReadySlotTexturePath(enemySlotTextureBase);
    const std::wstring enemyReady1 = GetReadySlotTexturePath(enemySlotTextureBase + 1);

    const bool myIsReady = gm.IsReady();

    if (myIsReady)
    {
        if (!myReady0.empty()) myUserSlot0->ChangeTexture(myReady0);
        if (!myReady1.empty()) myUserSlot1->ChangeTexture(myReady1);
    }
    else
    {
        if (!mySelected0.empty()) myUserSlot0->ChangeTexture(mySelected0);
        if (!mySelected1.empty()) myUserSlot1->ChangeTexture(mySelected1);
    }

    if (!myWeapon0.empty()) { myWeaponSlot0->ChangeTexture(myWeapon0); myWeaponSlot0->SetScale(XMFLOAT3(1.f, 1.f, 1.f)); }
    if (!myWeapon1.empty()) { myWeaponSlot1->ChangeTexture(myWeapon1); myWeaponSlot1->SetScale(XMFLOAT3(1.f, 1.f, 1.f)); }



    const bool enemyReady = gm.IsOpponentReady();
    if (!enemyReady0.empty() && enemyReady) enemyUserSlot0->ChangeTexture(enemyReady0);
    else if (!enemySelected0.empty()) enemyUserSlot0->ChangeTexture(enemySelected0);

    if (!enemyReady1.empty() && enemyReady) enemyUserSlot1->ChangeTexture(enemyReady1);
    else if (!enemySelected1.empty()) enemyUserSlot1->ChangeTexture(enemySelected1);
    if (!enemyWeapon0.empty()) { enemyWeaponSlot0->ChangeTexture(enemyWeapon0); enemyWeaponSlot0->SetScale(XMFLOAT3(1.f, 1.f, 1.f)); }
    if (!enemyWeapon1.empty()) { enemyWeaponSlot1->ChangeTexture(enemyWeapon1); enemyWeaponSlot1->SetScale(XMFLOAT3(1.f, 1.f, 1.f)); }
}

void WeaponSelectScene::Update(float dt)
{
    GameManager& gm = GameManager::Get();

    if (gm.GetSceneState() == CurrentSceneState::CountDown || gm.IsCountdownActive())
    {
        ApplyCountDownUnits();
    }
    else
    {
        ApplyOpponentPresenceVisuals();
        ApplyReadyStateVisuals();
    }
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
