#pragma once

#include "SceneState.h"



class GameManager
{
public:
    void SetSceneState(CurrentSceneState state);
    void StartCountDown(int countTime,int S1U1,int S1U2,int S2U1,int S2U2);
    void Tick(float dt);

private:
    CurrentSceneState m_state = CurrentSceneState::Title;

    bool m_countdownActive = false;
    float m_countdownRemaining = 0.0f;

    int m_S1U1 = 0;
    int m_S1U2 = 0;
    int m_S2U1 = 0;
    int m_S2U2 = 0;
};
