#include "pch.h"
#include "GuideScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"

bool GuideScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);

    auto win = YunoEngine::GetWindow();
    ClientW = (float)win->GetClientWidth();
    ClientH = (float)win->GetClientHeight();

    CreateWidget<TextureImage>(L"Option", L"../Assets/UI/TITLE/option.png", XMFLOAT3(0, 0, 0));

    CreateMainUI();
    CreateKeywordUI();
    CreateCardTypeUI();
    CreateTurnUI();
    CreateCoinTossUI();

    ChangeUIState(GuideUIState::Main);

    return true;
}

void GuideScene::CreateMainUI()
{//TODO::위치 잡아줘
    const float baseX = ((ClientW-630) / 2)-2;
    const float baseY = ClientH / 2 - 80;

    auto makePos = [&](int index)
        {
            return XMFLOAT3(
                baseX,
                baseY - 200.f + index * 100.f,
                0.f
            );
        };

    m_keywordBtn = CreateWidget<OptionButton>(
        L"KeywordBtn",
        Float2(208.5f, 45),
        makePos(0),
        UIDirection::Center
    );
    m_keywordBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/guide_cardkeyword_mouseout.png",
        L"../Assets/UI/TITLE/guide_cardkeyword_mouseover.png"
    );
    m_keywordBtn->SetOnClick([this]()
        {
            ChangeUIState(GuideUIState::Keyword);
        });

    m_cardTypeBtn = CreateWidget<OptionButton>(
        L"CardTypeBtn",
        Float2(208.5f, 45),
        makePos(1),
        UIDirection::Center
    );
    m_cardTypeBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/guide_cardtype_mouseout.png",
        L"../Assets/UI/TITLE/guide_cardtype_mouseover.png"
    );
    m_cardTypeBtn->SetOnClick([this]()
        {
            ChangeUIState(GuideUIState::CardType);
        });

    m_turnBtn = CreateWidget<OptionButton>(
        L"TurnBtn",
        Float2(208.5f, 45),
        makePos(2),
        UIDirection::Center
    );
    m_turnBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/guide_turn_mouseout.png",
        L"../Assets/UI/TITLE/guide_turn_mouseover.png"
    );
    m_turnBtn->SetOnClick([this]()
        {
            std::cout << "test";
            ChangeUIState(GuideUIState::Turn);
        });

    m_coinTossBtn = CreateWidget<OptionButton>(
        L"CoinTossBtn",
        Float2(208.5f, 45),
        makePos(3),
        UIDirection::Center
    );
    m_coinTossBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/guide_cointoss_mouseout.png",
        L"../Assets/UI/TITLE/guide_cointoss_mouseover.png"
    );
    m_coinTossBtn->SetOnClick([this]()
        {
            std::cout << "test";
            ChangeUIState(GuideUIState::CoinToss);
        });

    //Back//TODO::위치
    m_backBtn = CreateWidget<SceneChangeButton>(
        L"BackBtn",
        Float2(416, 90),
        XMFLOAT3(ClientW / 2 + 315, ClientH / 2 + 350, 0),
        UIDirection::Center
    );
    m_backBtn->SetTargetScene(CurrentSceneState::Title);
    m_backBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/back_mouseout.png",
        L"../Assets/UI/TITLE/back_mouseover.png"
    );
}

void GuideScene::CreateKeywordUI()
{
    m_keywordRoot = CreateWidget<TextureImage>(
        L"KeywordPanel",
        L"../Assets/UI/TITLE/guide_cardkeyword.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 -10, 0),
        UIDirection::Center
    );
    m_keywordRoot->SetVisible(Visibility::Visible);
}

void GuideScene::CreateCardTypeUI()
{
    m_cardTypeRoot = CreateWidget<TextureImage>(
        L"CardTypePanel",
        L"../Assets/UI/TITLE/guide_cardtype.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 - 10, 0),
        UIDirection::Center
    );
    m_cardTypeRoot->SetVisible(Visibility::Hidden);
}

void GuideScene::CreateTurnUI()
{
    m_turnRoot = CreateWidget<TextureImage>(
        L"TurnPanel",
        L"../Assets/UI/TITLE/guide_turn.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 - 10, 0),
        UIDirection::Center
    );
    m_turnRoot->SetVisible(Visibility::Hidden);
}

void GuideScene::CreateCoinTossUI()
{
    m_coinTossRoot = CreateWidget<TextureImage>(
        L"CoinTossPanel",
        L"../Assets/UI/TITLE/guide_cointoss.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 - 10, 0),
        UIDirection::Center
    );
    m_coinTossRoot->SetVisible(Visibility::Hidden);
}

void GuideScene::ClearSelection()
{
    m_keywordBtn->SetSelected(false);
    m_cardTypeBtn->SetSelected(false);
    m_turnBtn->SetSelected(false);
    m_coinTossBtn->SetSelected(false);
}

void GuideScene::ClearRoot()
{
    m_keywordRoot->SetVisible(Visibility::Hidden);
    m_cardTypeRoot->SetVisible(Visibility::Hidden);
    m_turnRoot->SetVisible(Visibility::Hidden);
    m_coinTossRoot->SetVisible(Visibility::Hidden);
}

void GuideScene::ChangeUIState(GuideUIState state)
{
    ClearSelection();
    ClearRoot();    

    m_uiState = state;

    switch (state)
    {
    case GuideUIState::Main:
        m_keywordBtn->SetVisible(Visibility::Visible);
        m_cardTypeBtn->SetVisible(Visibility::Visible);
        m_turnBtn->SetVisible(Visibility::Visible);
        m_coinTossBtn->SetVisible(Visibility::Visible);
        m_keywordRoot->SetVisible(Visibility::Visible);
        m_keywordBtn->SetSelected(true);
        break;

    case GuideUIState::Keyword:
        m_keywordBtn->SetSelected(true);
        m_keywordRoot->SetVisible(Visibility::Visible);
        break;

    case GuideUIState::CardType:
        m_cardTypeBtn->SetSelected(true);
        m_cardTypeRoot->SetVisible(Visibility::Visible);
        break;

    case GuideUIState::Turn:
        m_turnBtn->SetSelected(true);
        m_turnRoot->SetVisible(Visibility::Visible);
        break;

    case GuideUIState::CoinToss:
        m_coinTossBtn->SetSelected(true);
        m_coinTossRoot->SetVisible(Visibility::Visible);
        break;
    }
}

void GuideScene::OnDestroyScene()
{
}

void GuideScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void GuideScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void GuideScene::Update(float dt)
{
    SceneBase::Update(dt);

}

void GuideScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void GuideScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
