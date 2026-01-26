#include "pch.h"

#include "GameManager.h"

#include "YunoEngine.h"
#include "ISceneManager.h"

#include "WeaponSelectScene.h"
#include "PlayScene.h"

void GameManager::SetSceneState(CurrentSceneState state)
{
    ISceneManager* sm = YunoEngine::GetSceneManager();
    if (!sm) return;

    switch (state) 
    {
    case CurrentSceneState::Title:
    {
        break;
    }
    case CurrentSceneState::GameStart:
    {
        SceneTransitionOptions opt{};
        opt.immediate = false;
        sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);
        break;
    }
    case CurrentSceneState::CountDown:
    {
        // 여기서 상대가 무슨 유닛 골랐는지 보이게 텍스쳐 변경

        break;
    }
    case CurrentSceneState::RoundStart:
    {
        SceneTransitionOptions opt{};
        opt.immediate = false;
        sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);
        break;
    }
    case CurrentSceneState::SubmitCard:
    {
        break;
    }
    case CurrentSceneState::AutoBattle:
    {
        break;
    }
    case CurrentSceneState::RoundEnd:
    {
        break;
    }
    case CurrentSceneState::GameEnd:
    {
        break;
    }
    default:
        assert(false,"No state Scene");



    }
}

void GameManager::StartCountDown(int countTime, int S1U1, int S1U2, int S2U1, int S2U2)
{
    // 상대방 슬롯에 전달받은 유닛ID에 맞는 텍스쳐로 변경하기 넣으면 됨
    // + 카운트 다운 3초 시작
    // 3초 후에 RoundStart씬으로 이동하면서 데이터초기화
    m_S1U1 = S1U1;
    m_S1U2 = S1U2;
    m_S2U1 = S2U1;
    m_S2U2 = S2U2;

    m_countdownActive = true;
    m_countdownRemaining = static_cast<float>(countTime);

    SetSceneState(CurrentSceneState::CountDown);
}

void GameManager::Tick(float dt)
{
    if (m_countdownActive)
    {
        m_countdownRemaining -= dt;
        if (m_countdownRemaining <= 0.0f)
        {
            m_countdownActive = false;
            m_countdownRemaining = 0.0f;

            SetSceneState(CurrentSceneState::RoundStart);
        }
    }
}
