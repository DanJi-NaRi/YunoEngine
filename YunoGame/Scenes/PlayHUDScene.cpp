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
    imojibox->Attach(CreateWidget<TextureImage>(L"Imoji", L"../Assets/UI/PLAY/Imoji_zZZ_mouseout.png", XMFLOAT3(0, 0, 0), UIDirection::Center));
    imojibox->Attach(CreateWidget<TextureImage>(L"Imoji", L"../Assets/UI/PLAY/Imoji_JiantAngry_mouseout.png", XMFLOAT3(0, 0, 0), UIDirection::Center));
    imojibox->Attach(CreateWidget<TextureImage>(L"Imoji", L"../Assets/UI/PLAY/Imoji_EZ_mouseout.png", XMFLOAT3(0, 0, 0), UIDirection::Center));

    imojibox = CreateWidget<TextureImage>(L"ImojiBubble", L"../Assets/UI/PLAY/SpeechBubble.png", XMFLOAT3(0, 0, 0), UIDirection::Center);
    imojibox->Attach(CreateWidget<TextureImage>(L"Imoji", L"../Assets/UI/PLAY/Imoji_zZZ_mouseout.png", XMFLOAT3(0, 0, 0), UIDirection::Center));
    imojibox = CreateWidget<TextureImage>(L"ImojiBubble", L"../Assets/UI/PLAY/SpeechBubble.png", XMFLOAT3(0, 0, 0), UIDirection::Center);
    imojibox->Attach(CreateWidget<TextureImage>(L"Imoji", L"../Assets/UI/PLAY/Imoji_zZZ_mouseout.png", XMFLOAT3(0, 0, 0), UIDirection::Center));

    return true;
}

void PlayHUDScene::OnDestroyScene()
{
    //std::cout << "[PlayHUDScene] OnDestroy\n";

}

void PlayHUDScene::OnEnter()
{
    //std::cout << "[PlayHUDScene] OnEnter\n"; 
}

void PlayHUDScene::OnExit()
{
    //std::cout << "[PlayHUDScene] OnExit\n"; 
}

void PlayHUDScene::ShowEmoteImage(uint8_t pid, uint8_t emoteId)
{
    std::cout << "[PlayScene] ShowEmoteImage pid=" << int(pid)
        << " emoteId=" << int(emoteId) << "\n";

    auto* emoji = CreateWidget<Emoji>(
        L"Emoji",
        Float2{ 64, 64 },
        (pid == 1) ? XMFLOAT3{ 300, 200, 0 } : XMFLOAT3{ 800, 200, 0 }
    );

    emoji->SetLayer(WidgetLayer::HUD);
    emoji->ChangeMaterial(emoteId);
    m_emojis.push_back({ emoji, 2.0f });
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
    for (auto it = m_emojis.begin(); it != m_emojis.end(); )
    {
        it->remainTime -= dt;

        if (it->remainTime <= 0.f)
        {
            // UI 매니저에서 제거
            m_uiManager->DestroyWidget(it->widget->GetID());
            it = m_emojis.erase(it);
        }
        else
        {
            ++it;
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
