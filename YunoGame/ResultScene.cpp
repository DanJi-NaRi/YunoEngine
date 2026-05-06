#include "pch.h"
#include "ResultScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"
#include "ResultPanel.h"
#include "AudioQueue.h"

bool ResultScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);

    CreateUI();

    ChangeUIState(ResultUIState::Main);
    return true;
}

void ResultScene::CreateUI()
{
    const float baseX = ClientW / 2 + 960;
    const float baseY = ClientH / 2 + 540;

    // --------------------------------------Victory 생성--------------------------------------
    m_victoryPanel = CreateWidget<ResultPanel>(
        L"VictoryPanel",
        Float2(ClientW, ClientH),
        XMFLOAT3(baseX, baseY, 0),
        UIDirection::Center
    );

    m_victoryPanel->SetImages(
        L"../Assets/UI/ENDING/victory_1.png",
        L"../Assets/UI/ENDING/victory_2.png"
    );

    m_victoryPanel->Hide();


    // --------------------------------------Defeat 생성--------------------------------------
    m_defeatPanel = CreateWidget<ResultPanel>(
        L"DefeatPanel",
        Float2(ClientW, ClientH),
        XMFLOAT3(baseX, baseY, 0),
        UIDirection::Center
    );

    m_defeatPanel->SetImages(
        L"../Assets/UI/ENDING/defeat_1.png",
        L"../Assets/UI/ENDING/defeat_2.png"
    );

    m_defeatPanel->Hide();


    // --------------------------------------Draw 생성--------------------------------------
    m_drawPanel = CreateWidget<ResultPanel>(
        L"DrawPanel",
        Float2(ClientW, ClientH),
        XMFLOAT3(baseX, baseY, 0),
        UIDirection::Center
    );

    m_drawPanel->SetImages(
        L"../Assets/UI/ENDING/draw_1.png",
        L"../Assets/UI/ENDING/draw_2.png"
    );

    m_drawPanel->Hide();
    
    // --------------------------------------TitleBtn 생성--------------------------------------
    m_titleBtn = CreateWidget<OptionButton>(
        L"TitleBtn",
        Float2(1538, 105),
        XMFLOAT3(baseX, baseY + 300, 0),
        UIDirection::Center
    );
    m_titleBtn->SetHoverTexture(
        L"../Assets/UI/ENDING/back_to_title_mouseout.png",
        L"../Assets/UI/ENDING/back_to_title_mouseover.png"
    );
    m_titleBtn->SetOnClick([this]()
        {
            GameManager::Get().SetSceneState(CurrentSceneState::Title);
        });
}

void ResultScene::ChangeUIState(ResultUIState state)
{
    m_uiState = state;

    m_victoryPanel->Hide();
    m_defeatPanel->Hide();
    m_drawPanel->Hide();

    switch(state)
    {
    case ResultUIState::Victory:
        m_victoryPanel->Show();
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_Win));
        break;

    case ResultUIState::Defeat:
        m_defeatPanel->Show();
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_Lose));
        break;

    case ResultUIState::Draw:
        //std::cout << "HelpHelpHelpHelpHelpHelpHelpHelpHelpHelpDRAWDRAWDRAWDRAWDRAWDRAW";
        m_drawPanel->Show();
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_Draw));
        break;

    default:
        break;
    }
}

void ResultScene::OnDestroyScene()
{
}

void ResultScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);

    winner = GameManager::Get().GetWinnerPID();
    m_pID = GameManager::Get().GetPID();
    std::cout << winner;
    std::cout << m_pID;

    if (winner < 0)
        m_pendingState = ResultUIState::Draw;
    else if (winner == m_pID)
        m_pendingState = ResultUIState::Victory;
    else
        m_pendingState = ResultUIState::Defeat;

    m_delayTimer = 0.f;
    m_waiting = true;
}

void ResultScene::OnExit()
{
    m_victoryPanel->Hide();
    m_defeatPanel->Hide();
    m_drawPanel->Hide();
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void ResultScene::Update(float dt)
{
    SceneBase::Update(dt);

    if (m_waiting)
    {
        m_delayTimer += dt;

        if (m_delayTimer >= m_delayDuration)
        {
            m_waiting = false;
            ChangeUIState(m_pendingState);
        }
    }
}

void ResultScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void ResultScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
