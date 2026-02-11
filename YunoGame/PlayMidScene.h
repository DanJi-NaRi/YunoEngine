#pragma once

#include "SceneBase.h"
#include "PlayMidScene_InputContext.h"

#include "GameManager.h"

class TextureImage;
class AddCardPanel;

enum class PlayMidUIState
{
    Main,
    RoundStart,
    RevealCard,
    Warning,
    AddCardSelect,
    Finished
};

struct CardAnim
{
    TextureImage* card;
    XMFLOAT3 startPos;
    XMFLOAT3 targetPos;
};

class PlayMidScene final : public SceneBase
{
public:

    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "PlayMidScene"; };

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    // UI 생성
    void CreateMainUI();
    void CreateRoundStartUI();
    void CreateRevealCardUI();
    void CreateWarningUI();
    void CreateAddCardSelectUI();

    // 상태
    void ChangeUIState(PlayMidUIState state);
    void Clear();

    // 함수
    void CheckRoundChange(GameManager& gm);
    void UpdateRoundStart(float dt);
    void UpdateRevealCard(float dt, GameManager& gm);
    bool PopAndRevealPair(GameManager& gm);
    void RevealSingleCard(const BattleResult& br, GameManager& gm);
    void ResetAllCardsToBack();
    void UpdateWarning(float dt);
private:
    PlayMidScene_InputContext m_uiCtx;
    PlayMidUIState m_uiState = PlayMidUIState::Main;
    
    bool m_revealFinished = false;
    float m_phaseTimer = 0.f;
    float m_revealTimer = 0.f;
    float m_postRevealTimer = 0.f;
    float m_postRevealDelay = 4.0f;
    float m_revealInterval = 5.0f; // 5.0초마다 한 장
    bool  m_roundNoticeShown = false; // 라운드 시작 알림 한번만

    float ClientW = 0.f;
    float ClientH = 0.f;

    int round = 0;

    int m_leftRevealIndex = 0;
    int m_rightRevealIndex = 0;

    TextureImage* m_round;
    std::vector<TextureImage* > m_readyCards;
    std::vector<CardAnim> m_cardAnims;
    
    float m_moveTimer = 0.f;
    float m_moveDuration = 0.5f;   // 0.5초 이동
    bool m_isMoving = false;

    float m_warningTimer = 0.f;
    float m_warningDuration = 2.5f;

    float m_warningScrollSpeed = 400.f;   // 초당 이동 속도
    float m_warningStartX = -500.f;       // 시작 위치
    float m_warningEndX = 2500.f;         // 화면 밖 오른쪽

    TextureImage* m_leftBG = nullptr;
    TextureImage* m_rightBG = nullptr;

    TextureImage* m_warningBG;
    TextureImage* m_warningFG;
    TextureImage* m_warningLogo;

    AddCardPanel* m_addCardPanel;
};
