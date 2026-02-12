#pragma once

#include "SceneBase.h"
#include "PlayHUD_InputContext.h"

class Emoji;
class TextureImage;
class EmoteButton;
class PlayerIcon;
class SpriteSheet;

struct TimedEmoji
{
    uint8_t pid = 0;
    uint8_t emoteid = 0;
    float remainTime; // 초
};

class PlayHUDScene final : public SceneBase
{
public:

    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "PlayHUDScene"; };

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

    void ShowEmoteImage(uint8_t pid, uint8_t emoteId); //YDM TEST IMOTE
    void TryInitPlayerIconsFromWeaponData();

    void UpdateWData(float dTime);
    void RefreshTurnTexture();

    bool CheckRoundOver();
    void ResetRound();

private:
    PlayHUD_InputContext m_uiContext;

    bool m_1PAllDead = false;
    bool m_2PAllDead = false;

    bool isRoundReset = false;

    int curRound = 0;
    int prevRound = 0;

    SpriteSheet* m_SceneChange;
    bool isChangeRound = false;

    void ChangeRound(float dt);

    Widget* m_pTurn = nullptr;    // 1의 자리수
    Widget* m_pTurn10 = nullptr;  // 10의 자리수
    uint64_t m_appliedTurnStateVersion = 0;

    std::array<PlayerIcon*, 4> m_playerIcons{};
    uint32_t m_appliedWeaponDataVersion = 0;

    std::array<TextureImage*, 3> roundWin{};

    TextureImage* m_EmoteBubble1P = nullptr;
    TextureImage* m_EmoteBubble2P = nullptr;
    Emoji* m_EmoteImage1P = nullptr;
    Emoji* m_EmoteImage2P = nullptr;
    std::vector<Emoji*> m_emoteImages;

    std::vector<EmoteButton*> m_emoteButtons;

    std::vector<TimedEmoji*> m_emojis; // 이모지 시간 관리
    TimedEmoji m_emoji1P;
    TimedEmoji m_emoji2P;
};
