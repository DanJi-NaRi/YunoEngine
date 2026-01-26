#pragma once

#include "SceneState.h"
#include "CardData.h"

class ISceneManager;
class ObjectManager;


// 얘가 뭘 가지고있어야 될까?
class GameManager
{
public:
    static void Initialize(GameManager* inst);
    static void Shutdown();
    static GameManager& Get();

    void BindSceneManager(ISceneManager* sm) { m_sceneManager = sm; }

    void SetSceneState(CurrentSceneState state);
    void StartCountDown(int countTime,int S1U1,int S1U2,int S2U1,int S2U2);
    void Tick(float dt);

    void SendPacket(std::vector<std::uint8_t>&& bytes);

    void SetSlotIdx(int idx) { m_mySlot = idx; };
    int GetSlotiIdx() { return m_mySlot; };

    PieceType GetMyPiece(int idx) { return m_myPick[idx]; };

    bool ToggleReady();
    bool IsReady() const { return m_isReady; }

private:
    static GameManager* s_instance;

private:
    ISceneManager* m_sceneManager = nullptr;

    CurrentSceneState m_state = CurrentSceneState::Title;

    PieceType m_myPick[2] = { PieceType::None, PieceType::None };

    int m_mySlot = -1; // 0 또는 1  >> 0이면 왼쪽 1이면 오른쪽

    bool m_isReady = false;

    bool m_countdownActive = false;
    float m_countdownRemaining = 0.0f;

    int m_S1U1 = 0;
    int m_S1U2 = 0;
    int m_S2U1 = 0;
    int m_S2U2 = 0;
};
