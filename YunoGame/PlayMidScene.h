#pragma once

#include "SceneBase.h"
#include "PlayMidScene_InputContext.h"

#include "GameManager.h"

class TextureImage;
class AddCardPanel;
class SpriteSheet;

enum class PlayMidUIState
{
    Main,
    RoundStart,
    RevealCard,
    Warning,
    AddCardSelect,
    CoinToss,
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
    void CreateCoinTossUI();
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
    void UpdateCoinToss(float dt);
    bool TryHandleCoinToss(GameManager& gm);
private:
    PlayMidScene_InputContext m_uiCtx;
    PlayMidUIState m_uiState = PlayMidUIState::Main;
    
    bool m_revealFinished = false;
    float m_phaseTimer = 0.f;
    float m_revealTimer = 0.f;
    float m_postRevealTimer = 0.f;
    float m_postRevealDelay = 8.0f;
    float m_nextRevealDelay = 0.f; // 다음 오픈까지 남은 시간
    bool m_waitingNextReveal = false; // 대기중 여부
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
    float m_warningStartX = -300.f;       // 시작 위치
    float m_warningEndX = 2500.f;         // 화면 밖 오른쪽

    SpriteSheet* m_coin = nullptr;

    int coin = 0;

    int m_coinPairIndex = 0;
    float m_coinTimer = 0.f;
    float m_coinDuration = 2.0f;
    bool  m_coinPlaying = false;

    bool m_isRevealInitialized = false;
    bool m_coinHandledThisStep = false;

    TextureImage* m_leftBG = nullptr;
    TextureImage* m_rightBG = nullptr;

    TextureImage* m_warningBG;
    TextureImage* m_warningFG;
    TextureImage* m_warningLogo;

    AddCardPanel* m_addCardPanel;
};
