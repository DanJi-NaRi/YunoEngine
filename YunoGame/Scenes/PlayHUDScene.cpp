#include "pch.h"

// 본인 씬 최상단 ㄱㄱ
#include "PlayHUDScene.h"
// 다음 엔진
#include "YunoEngine.h"
// 다음 오브젝트 매니저 여기까지 고정
#include "ObjectManager.h"
// 게임 매니저
#include "GameManager.h"
#include "UIManager.h"
#include "iwindow.h"

// 여러 오브젝트들 ;; 
#include "UIWidgets.h"
#include "HealthBar.h"
#include "StaminaBar.h"
#include "EmoteButton.h"


bool PlayHUDScene::OnCreateScene()
{
    auto iwindow = YunoEngine::GetWindow();
    float ClientW = static_cast<float>(iwindow->GetClientWidth());
    float ClientH = static_cast<float>(iwindow->GetClientHeight());

    m_uiManager->SetOrthoFlag(true);

    // BackGround
    CreateWidget<TextureImage>(L"BackGround", L"../Assets/UI/PLAY/Play_background_fade.png", XMFLOAT3(0, 0, 0));

    // Player
    CreateWidget<TextureImage>(L"P1", L"../Assets/UI/PLAY/Play_background_1P_name.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"P2", L"../Assets/UI/PLAY/Play_background_2P_name.png", XMFLOAT3(0, 0, 0));

    // Turn
    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Play_background_turn.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Round_0.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"Turn", L"../Assets/UI/PLAY/Round_2.png", XMFLOAT3(0, 0, 0));

    //WinLose
    CreateWidget<TextureImage>(L"RoundCount", L"../Assets/UI/PLAY/3player_yet.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"RoundCount", L"../Assets/UI/PLAY/3player_yet.png", XMFLOAT3(0, 0, 0));
    CreateWidget<TextureImage>(L"RoundCount", L"../Assets/UI/PLAY/3player_yet.png", XMFLOAT3(0, 0, 0));

    auto icon = CreateWidget<PlayerIcon>(L"PlayerIcon", Float2(108, 94), XMFLOAT3(700, 500, 0), UIDirection::Center);
    icon->SetPlayer(1);
    icon = CreateWidget<PlayerIcon>(L"PlayerIcon", Float2(108, 94), XMFLOAT3(700, 500, 0), UIDirection::Center);
    icon->SetPlayer(1);
    icon = CreateWidget<PlayerIcon>(L"PlayerIcon", Float2(108, 94), XMFLOAT3(700, 500, 0), UIDirection::Center);
    icon->SetPlayer(1);
    icon = CreateWidget<PlayerIcon>(L"PlayerIcon", Float2(108, 94), XMFLOAT3(700, 500, 0), UIDirection::Center);
    icon->SetPlayer(1);

    auto imojibox = CreateWidget<TextureImage>(L"ImojiBox", L"../Assets/UI/PLAY/ImojiBox.png", XMFLOAT3(0, 0, 0), UIDirection::Center);
    auto emote = CreateWidget<EmoteButton>(L"Imoji", Float2(80, 67), XMFLOAT3(0, 0, 0), UIDirection::LeftTop);
    emote->SetIdleHoverTextuer(L"../Assets/UI/PLAY/Imoji_zZZ_mouseout.png", L"../Assets/UI/PLAY/Imoji_zZZ_mouseover.png");
    emote->SetEmoteNum(1);
    auto emote2 = CreateWidget<EmoteButton>(L"Imoji", Float2(80, 67), XMFLOAT3(0, 0, 0), UIDirection::LeftTop);
    emote2->SetIdleHoverTextuer(L"../Assets/UI/PLAY/Imoji_JiantAngry_mouseout.png", L"../Assets/UI/PLAY/Imoji_JiantAngry_mouseover.png");
    emote2->SetEmoteNum(2);
    auto emote3 = CreateWidget<EmoteButton>(L"Imoji", Float2(80, 67), XMFLOAT3(0, 0, 0), UIDirection::LeftTop);
    emote3->SetIdleHoverTextuer(L"../Assets/UI/PLAY/Imoji_EZ_mouseout.png", L"../Assets/UI/PLAY/Imoji_EZ_mouseover.png");
    emote3->SetEmoteNum(3);
    imojibox->Attach(emote);
    imojibox->Attach(emote2);
    imojibox->Attach(emote3);

    m_emoteButtons.push_back(emote);
    m_emoteButtons.push_back(emote2);
    m_emoteButtons.push_back(emote3);

    m_EmoteBubble1P = CreateWidget<TextureImage>(L"ImojiBubble", L"../Assets/UI/PLAY/SpeechBubble.png", XMFLOAT3(0, 0, 0), UIDirection::Center);
    m_EmoteBubble1P->SetVisible(Visibility::Hidden);
    m_EmoteBubble2P = CreateWidget<TextureImage>(L"ImojiBubble", L"../Assets/UI/PLAY/SpeechBubble.png", XMFLOAT3(0, 0, 0), UIDirection::Center);
    m_EmoteBubble2P->SetVisible(Visibility::Hidden);

    m_EmoteImage1P = CreateWidget<Emoji>(L"emote", Float2(80, 67), XMFLOAT3(7, 15, 0), UIDirection::Center);
    m_EmoteImage1P->SetScale({-1, 1, 1});
    m_EmoteImage1P->SetVisible(Visibility::Hidden);
    m_EmoteImage1P->SetLayer(WidgetLayer::HUD);
    m_EmoteImage2P = CreateWidget<Emoji>(L"emote", Float2(80, 67), XMFLOAT3(7, 15, 0), UIDirection::Center);
    m_EmoteImage2P->SetVisible(Visibility::Hidden);
    m_EmoteImage2P->SetLayer(WidgetLayer::HUD);

    m_EmoteBubble1P->Attach(m_EmoteImage1P);
    m_EmoteBubble2P->Attach(m_EmoteImage2P);

    m_emoteImages.push_back(m_EmoteImage1P);
    m_emoteImages.push_back(m_EmoteImage2P);

    m_emojis.push_back(&m_emoji1P);
    m_emojis.push_back(&m_emoji2P);

    return true;
}

void PlayHUDScene::OnDestroyScene()
{
    //std::cout << "[PlayHUDScene] OnDestroy\n";

}

void PlayHUDScene::OnEnter()
{
    //std::cout << "[PlayHUDScene] OnEnter\n"; 
    YunoEngine::GetInput()->AddContext(&m_uiContext, this);
}

void PlayHUDScene::OnExit()
{
    //std::cout << "[PlayHUDScene] OnExit\n"; 
    YunoEngine::GetInput()->RemoveContext(&m_uiContext);
}

void PlayHUDScene::ShowEmoteImage(uint8_t pid, uint8_t emoteId)
{
    std::cout << "[PlayScene] ShowEmoteImage pid=" << int(pid)
        << " emoteId=" << int(emoteId) << "\n";

    if ((pid == 1 && m_emoji1P.emoteid) || (pid == 2 && m_emoji2P.emoteid))
        return;

    auto& emote = m_emoteImages[pid - 1];
    emote->ChangeMaterial(emoteId);

    if (pid == 1)
    {
        m_emoji1P = { pid, emoteId, 2.0f };
        emote->SetVisible(Visibility::Visible);
        m_EmoteBubble1P->SetVisible(Visibility::Visible);
    }
    else if(pid == 2)
    {
        m_emoji2P = { pid, emoteId, 2.0f };
        emote->SetVisible(Visibility::Visible);
        m_EmoteBubble2P->SetVisible(Visibility::Visible);
    }

    if (GameManager::Get().GetPID() == pid)
    {
        for (auto* button : m_emoteButtons)
        {
            button->Lock();
        }
    }
}

void PlayHUDScene::Update(float dt)
{
    // 이거만 있으면 오브젝트 업데이트 됨 따로 업뎃 ㄴㄴ
    SceneBase::Update(dt);

    PendingEmote emote;
    while (GameManager::Get().PopEmote(emote))
    {
        ShowEmoteImage(emote.pid, emote.emoteId);
    }
    //이모지 수명 관리
    for (auto& emoji : m_emojis)
    {
        if (!emoji->emoteid) continue;

        emoji->remainTime -= dt;

        if (emoji->remainTime <= 0.f)
        {
            emoji->emoteid = 0;

            if (emoji->pid == 1)
            {
                m_EmoteBubble1P->SetVisible(Visibility::Hidden);
                m_EmoteImage1P->SetVisible(Visibility::Hidden);
            }
            else
            {
                m_EmoteBubble2P->SetVisible(Visibility::Hidden);
                m_EmoteImage2P->SetVisible(Visibility::Hidden);
            }
                
            if (GameManager::Get().GetPID() == emoji->pid)
            {
                for (auto* button : m_emoteButtons)
                {
                    button->UnLock();
                }
            }
        }
    }
}

void PlayHUDScene::SubmitObj()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitObj();
}

void PlayHUDScene::SubmitUI()
{
    // 이거만 있으면 오브젝트 렌더 됨 따로 서브밋 ㄴㄴ
    SceneBase::SubmitUI();
}
