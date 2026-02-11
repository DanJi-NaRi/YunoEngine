#include "pch.h"
#include "PlayMidScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"
#include "AddCardPanel.h"

bool PlayMidScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);

    auto win = YunoEngine::GetWindow();
    ClientW = (float)win->GetClientWidth();
    ClientH = (float)win->GetClientHeight();

    CreateMainUI();
    CreateRoundStartUI();
    CreateRevealCardUI();
    CreateWarningUI();
    CreateAddCardSelectUI();

    ChangeUIState(PlayMidUIState::Main);

    return true;
}

void PlayMidScene::Clear()
{
    // 전부 비활성화
    if (m_round) m_round-> SetVisible(Visibility::Collapsed);

    for (auto* card : m_readyCards)
        if (card) card->SetVisible(Visibility::Collapsed);
    if (m_leftBG)  m_leftBG->SetVisible(Visibility::Collapsed);
    if (m_rightBG) m_rightBG->SetVisible(Visibility::Collapsed);

    if (m_warningBG) m_warningBG->SetVisible(Visibility::Collapsed);
    if (m_warningFG) m_warningFG->SetVisible(Visibility::Collapsed);
    if (m_warningLogo) m_warningLogo->SetVisible(Visibility::Collapsed);

    if (m_addCardPanel) m_addCardPanel->SetVisible(Visibility::Collapsed);
}

void PlayMidScene::CreateMainUI()
{
    // 기본 상태용 UI 필요하면 여기에
}

void PlayMidScene::CreateRoundStartUI()
{
    m_round = CreateWidget<TextureImage>(
        L"RoundNotice",
        L"../Assets/UI/PLAY/round1.png",
        XMFLOAT3(ClientW * 0.5f, ClientH * 0.5f, 0),
        UIDirection::Center
    );

    m_round->SetVisible(Visibility::Collapsed);
}

void PlayMidScene::CreateRevealCardUI()
{
    m_readyCards.clear();

    const float leftX = 50.f;
    const float rightX = ClientW - 50.f;

    const float centerY = ClientH * 0.5f;

    const float startY = centerY - 180.f;
    const float gapY = 160.f;

    m_leftBG = CreateWidget<TextureImage>(
        L"LeftCardBG",
        Float2(3840.f, 2160.f),
        XMFLOAT3(960, 540, 0),
        UIDirection::Center
    );

    m_leftBG->ChangeTexture(L"../Assets/UI/PLAY/Play_background_1P.png");
    m_leftBG->SetVisible(Visibility::Collapsed);

    m_rightBG = CreateWidget<TextureImage>(
        L"RightCardBG",
        Float2(3840.f, 2160.f),
        XMFLOAT3(960, 540, 0),
        UIDirection::Center
    );

    m_rightBG->ChangeTexture(L"../Assets/UI/PLAY/Play_background_2P.png");
    m_rightBG->SetVisible(Visibility::Collapsed);

    for (int i = 0; i < 8; ++i)
    {
        float x = (i < 4) ? leftX : rightX;
        float y = startY + (i % 4) * gapY;

        auto* card = CreateWidget<TextureImage>(
            L"ReadyCard",
            Float2(205.f, 297.f),
            XMFLOAT3(x, y, 0),
            UIDirection::Center
        );

        card->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        card->SetVisible(Visibility::Collapsed);
        m_readyCards.push_back(card);
    }
}

void PlayMidScene::CreateWarningUI()
{
    m_warningBG = CreateWidget<TextureImage>(
        L"WarningBG",
        Float2(3840.f, 2160.f),
        XMFLOAT3(ClientW * 0.5f, ClientH * 0.5f, 0),
        UIDirection::Center
    );

    m_warningBG->SetLayer(WidgetLayer::Background);
    m_warningBG->ChangeTexture(L"../Assets/UI/PLAY/warning.png");

    m_warningFG = CreateWidget<TextureImage>(
        L"WarningFG",
        Float2(8431.f, 535.f),
        XMFLOAT3(ClientW * 0.5f - 4014, ClientH * 0.5f, 0),
        UIDirection::Center
    );

    m_warningFG->ChangeTexture(L"../Assets/UI/PLAY/warning_animation.png");

    m_warningLogo = CreateWidget<TextureImage>(
        L"WarningLogo",
        Float2(534.f, 557.f),
        XMFLOAT3(ClientW * 0.5f, ClientH * 0.5f - 100, 0),
        UIDirection::Center
    );

    m_warningLogo->SetLayer(WidgetLayer::HUD);
    m_warningLogo->ChangeTexture(L"../Assets/UI/PLAY/warning_blingbling.png");

    m_warningBG->SetVisible(Visibility::Collapsed);
    m_warningFG->SetVisible(Visibility::Collapsed);
    m_warningLogo->SetVisible(Visibility::Collapsed);
}

void PlayMidScene::CreateAddCardSelectUI()
{
    m_addCardPanel = CreateWidget<AddCardPanel>(
        L"AddCardPanel",
        Float2{ ClientW, ClientH },
        XMFLOAT3(0, 0, 0),
        UIDirection::Center
    );
    m_addCardPanel->ChangeTexture(L"../Assets/UI/TITLE/volume_fill_full.png");
    m_addCardPanel->SetVisible(Visibility::Collapsed);
}

void PlayMidScene::ChangeUIState(PlayMidUIState state)
{
    if (m_uiState == state) return;

    Clear();

    m_uiState = state;
    m_phaseTimer = 0.f;

    switch (m_uiState)
    {
    case PlayMidUIState::Main:
        break;

    case PlayMidUIState::RoundStart:
        if (m_round)
            m_round->SetVisible(Visibility::Visible);
        break;

    case PlayMidUIState::RevealCard:
        m_cardAnims.clear();
        m_moveTimer = 0.f;
        m_isMoving = true;

        m_leftRevealIndex = 0;
        m_rightRevealIndex = 0;
        m_revealTimer = 0.f;
        m_revealFinished = false;
        m_postRevealTimer = 0.f;

        if (m_leftBG)  m_leftBG->SetVisible(Visibility::Visible);
        if (m_rightBG) m_rightBG->SetVisible(Visibility::Visible);

        for (int i = 0; i < m_readyCards.size(); ++i)
        {
            auto* card = m_readyCards[i];
            if (!card) continue;

            card->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
            card->SetVisible(Visibility::Visible);

            XMFLOAT3 target = card->GetPos();

            XMFLOAT3 start = target;

            if (i < 4)
                start.x = -300.f;                 // 왼쪽 화면 밖
            else
                start.x = ClientW + 300.f;        // 오른쪽 화면 밖

            card->SetPos(start);

            m_cardAnims.push_back({ card, start, target });
        }
        break;

    case PlayMidUIState::Warning:
        m_warningTimer = 0.f;

        if (m_warningBG) m_warningBG->SetVisible(Visibility::Visible);

        if (m_warningFG)
        {
            m_warningFG->SetVisible(Visibility::Visible);
            m_warningFG->SetPos({ m_warningStartX, ClientH * 0.5f, 0 });
        }

        if (m_warningLogo) m_warningLogo->SetVisible(Visibility::Visible);

        break;

    case PlayMidUIState::AddCardSelect:
    {
        if (m_addCardPanel)
            m_addCardPanel->SetVisible(Visibility::Visible);

        auto& gm = GameManager::Get();
        const auto& candidates = gm.GetDrawCandidates();

        m_addCardPanel->SetCandidateCards(candidates);
        m_addCardPanel->Show();
    }
    break;

    case PlayMidUIState::Finished:
        break;
    }
}

void PlayMidScene::OnDestroyScene()
{
}

void PlayMidScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
    //여기서 카드 결과 받을 거임
}

void PlayMidScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void PlayMidScene::CheckRoundChange(GameManager& gm)
{
    int cur = gm.GetCurrentRound();
    if (round == cur) return;

    round = cur;

    switch (round)
    {
    case 1: m_round->ChangeTexture(L"../Assets/UI/PLAY/round1.png"); break;
    case 2: m_round->ChangeTexture(L"../Assets/UI/PLAY/round2.png"); break;
    case 3: m_round->ChangeTexture(L"../Assets/UI/PLAY/round3.png"); break;
    default: break;
    }

    ChangeUIState(PlayMidUIState::RoundStart);
}

void PlayMidScene::UpdateRoundStart(float dt)
{
    if (m_phaseTimer >= 2.0f)
    {
        ChangeUIState(PlayMidUIState::Main);
    }
}

void PlayMidScene::ResetAllCardsToBack()
{
    for (auto* card : m_readyCards)
    {
        if (!card) continue;
        card->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
    }
}

void PlayMidScene::UpdateRevealCard(float dt, GameManager& gm)
{
    if (m_isMoving)
    {
        m_moveTimer += dt;
        float t = std::clamp(m_moveTimer / m_moveDuration, 0.f, 1.f);

        for (auto& anim : m_cardAnims)
        {
            float x = anim.startPos.x +
                (anim.targetPos.x - anim.startPos.x) * t;

            anim.card->SetPos({ x, anim.targetPos.y, 0 });
        }

        if (t >= 1.f)
        {
            m_isMoving = false;  // 이동 완료
        }

        return;  // 이동 중에는 카드 공개 안 함
    }
    // 아직 공개 중이면
    if (!m_revealFinished)
    {
        m_revealTimer += dt;

        if (m_revealTimer >= m_revealInterval)
        {
            m_revealTimer = 0.f;

            PopAndRevealPair(gm);

            //  마지막 카드까지 공개됐는지 체크
            if (m_leftRevealIndex >= 4 && m_rightRevealIndex >= 4 )
            {
                m_revealFinished = true;
                m_postRevealTimer = 0.f;
            }
        }
    }
    else
    {
        //  전부 공개 완료 후 대기 시간
        m_postRevealTimer += dt;

        if (m_postRevealTimer >= m_postRevealDelay)
        {
            ResetAllCardsToBack();
            ChangeUIState(PlayMidUIState::AddCardSelect);
        }
    }
}

void PlayMidScene::UpdateWarning(float dt)
{
    m_warningTimer += dt;

    if (m_warningFG)
    {
        XMFLOAT3 pos = m_warningFG->GetPos();
        pos.x += m_warningScrollSpeed * dt;

        m_warningFG->SetPos(pos);
    }

    if (m_warningTimer >= m_warningDuration)
    {
        ChangeUIState(PlayMidUIState::Main);
    }
}

bool PlayMidScene::PopAndRevealPair(GameManager& gm)
{
    if (gm.IsEmptyRevealQueue())
        return false;

    // 첫 번째 (P1)
    BattleResult br1 = gm.PopRevealPacket();
    RevealSingleCard(br1, gm);

    // 두 번째 (P2)
    if (!gm.IsEmptyRevealQueue())
    {
        BattleResult br2 = gm.PopRevealPacket();
        RevealSingleCard(br2, gm);
    }

    return true;
}

void PlayMidScene::RevealSingleCard(const BattleResult& br, GameManager& gm)
{
    uint32_t runtimeID = br.runTimeCardID;
    uint32_t dataID = gm.GetCardDataID(runtimeID);

    std::wstring texturePath =
        gm.GetCardBasicManager().GetCardTexturePath(dataID);

    int targetIndex = -1;

    if (br.pId == 1)
        targetIndex = m_leftRevealIndex++;
    else
        targetIndex = 4 + m_rightRevealIndex++;

    if (targetIndex >= 0 && targetIndex < (int)m_readyCards.size())
    {
        m_readyCards[targetIndex]->ChangeTexture(texturePath);
    }
}

void PlayMidScene::Update(float dt)
{
    SceneBase::Update(dt);

    GameManager& gm = GameManager::Get();

    m_phaseTimer += dt;

    CheckRoundChange(gm);
    
    if (gm.ConsumeRevealStart())
    {
        ChangeUIState(PlayMidUIState::RevealCard);
    }

   //if (gm.ConsumeWarning())
   //{
   //    ChangeUIState(PlayMidUIState::Warning);
   //}
    
    if (m_input->IsKeyPressed('V'))
    {
        ChangeUIState(PlayMidUIState::Warning);
    }

    switch (m_uiState)
    {
    case PlayMidUIState::RoundStart:
        UpdateRoundStart(dt);
        break;

    case PlayMidUIState::RevealCard:
        UpdateRevealCard(dt, gm);
        break;
    case PlayMidUIState::Warning:
        UpdateWarning(dt);
        break;
    default:
        break;
    }
}

void PlayMidScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void PlayMidScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
