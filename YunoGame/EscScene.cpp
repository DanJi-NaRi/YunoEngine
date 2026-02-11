#include "pch.h"
#include "EscScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"
#include "CheckPanel.h"

bool EscScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);

    auto win = YunoEngine::GetWindow();
    ClientW = (float)win->GetClientWidth();
    ClientH = (float)win->GetClientHeight();

    CreateWidget<TextureImage>(L"Option", L"../Assets/UI/PLAY/Background_Opacity.png", XMFLOAT3(0, 0, 0));

    CreateMainUI();
    CreateCheckUI();

    return true;
}

void EscScene::CreateMainUI()
{
    const float baseX = ClientW / 2;
    const float baseY = ClientH / 2 + 50;

    auto makePos = [&](int index)
        {
            return XMFLOAT3(
                baseX,
                baseY - 200.f + index * 80,
                0.f
            );
        };

    ISceneManager* sm = YunoEngine::GetSceneManager();
    // --------------------------------------Leave 버튼 시작--------------------------------------
    m_leaveBtn = CreateWidget<OptionButton>(
        L"LeaveBtn",
        Float2(btnW, btnH),
        makePos(0),
        UIDirection::Center
    );

    m_leaveBtn->SetHoverTexture(
        L"../Assets/UI/PLAY/backtotitle_mouseout.png",
        L"../Assets/UI/PLAY/backtotitle_mouseover.png"
    );

    m_leaveBtn->SetOnClick([this]()
        {
            m_checkPanel->SetBackGround(
                L"../Assets/UI/PLAY/backtotitle.png"
            );

            m_checkPanel->SetYesAction([this]()
                {
                    GameManager::Get().SetSceneState(CurrentSceneState::Title);
                });

            m_checkPanel->SetNoAction([this]()
                {
                    m_checkPanel->Hide();
                });

            m_checkPanel->Show();
        });

     //--------------------------------------Guide 버튼 시작--------------------------------------
    m_guideBtn = CreateWidget<OptionButton>(
        L"GuideBtn",
        Float2(btnW, btnH),
        makePos(2),
        UIDirection::Center
    );

    m_guideBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/guide_mouseout.png",
        L"../Assets/UI/TITLE/guide_mouseover.png"
    );

    m_guideBtn->SetOnClick([this]()
        {
            GameManager::Get().SetSceneState(CurrentSceneState::Guide);
        });

    //--------------------------------------Option 버튼 시작--------------------------------------
    m_optionBtn = CreateWidget<OptionButton>(
        L"OptionBtn",
        Float2(btnW, btnH),
        makePos(1),
        UIDirection::Center
    );

    m_optionBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/option_mouseout.png",
        L"../Assets/UI/TITLE/option_mouseover.png"
    );

    m_optionBtn->SetOnClick([this]()
        {
            GameManager::Get().SetSceneState(CurrentSceneState::Option);
        });
    
     //--------------------------------------Surrender 버튼 시작--------------------------------------
    m_surrenderBtn = CreateWidget<OptionButton>(
        L"SurrenderBtn",
        Float2(btnW, btnH),
        makePos(3),
        UIDirection::Center
    );

    m_surrenderBtn->SetHoverTexture(
        L"../Assets/UI/PLAY/surren_mouseout.png",
        L"../Assets/UI/PLAY/surren_mouseover.png"
    );

    m_surrenderBtn->SetOnClick([this]()
        {
            m_checkPanel->SetBackGround(
                L"../Assets/UI/PLAY/surren.png"
            );

            m_checkPanel->SetYesAction([this]()
                {
                    GameManager::Get().SendSurrender();
                });

            m_checkPanel->SetNoAction([this]()
                {
                    m_checkPanel->Hide();
                });

            m_checkPanel->Show();
        });
    
    //--------------------------------------Back 버튼 시작--------------------------------------
    m_backBtn = CreateWidget<PopButton>(
        L"BackBtn",
        Float2(btnW*2, btnH*2),
        makePos(4),
        UIDirection::Center
    );
    m_backBtn->SetHoverTexture(
        L"../Assets/UI/PLAY/back_mouseout.png",
        L"../Assets/UI/PLAY/back_mouseover.png"
    );
}

void EscScene::CreateCheckUI()
{
    m_checkPanel = CreateWidget<CheckPanel>(
        L"CheckPanel",
        Float2(ClientW, ClientH),
        XMFLOAT3(ClientW / 2, ClientH / 2, 0),
        UIDirection::Center
    );

    m_checkPanel->ChangeTexture(L"../Assets/UI/PLAY/black_background.png");

    m_checkPanel->Hide();
}

void EscScene::OnDestroyScene()
{
}

void EscScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void EscScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void EscScene::Update(float dt)
{
    SceneBase::Update(dt);

}

void EscScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void EscScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
