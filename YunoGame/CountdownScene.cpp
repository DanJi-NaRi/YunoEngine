#include "pch.h"

#include "CountdownScene.h"
#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "CountdownImage.h"
#include "Emoji.h"


namespace
{
    constexpr float kVsSlideDuration = 0.6f;
}


bool CountdownScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);
    return true;
}

void CountdownScene::OnDestroyScene()
{
}

void CountdownScene::ShowOrUpdateCountdownImage(int num)
{
    CountdownImage* img = nullptr;
    auto canvas = m_uiManager->GetCanvasSize();
    XMFLOAT3 centerPos{
    canvas.x * 0.5f,
    canvas.y * 0.5f,
    0.0f
    };
    
    // 아직 위젯이 없으면 생성
    if (m_countdownWidgetID == UINT_MAX)
    {
        img = CreateWidget<CountdownImage>(
            L"Countdown",
            Float2{ 256, 256 }, 
            centerPos,
            Float2{ 0.5f, 0.5f }
        );

        img->SetLayer(WidgetLayer::HUD);
        m_countdownWidgetID = img->GetID();
    }
    else
    {
        img = static_cast<CountdownImage*>(
            m_uiManager->FindWidget(m_countdownWidgetID)
            );
    }        

    if (img)
        img->ChangeMaterial(num); // 3, 2, 1
}

void CountdownScene::StartVsSlide()
{
    auto canvas = m_uiManager->GetCanvasSize();
    const float centerY = canvas.y * 0.5f;
    const float centerX = canvas.x * 0.5f;

    if (m_countdownWidgetID != UINT_MAX)
    {
        if (auto* countdown = m_uiManager->FindWidget(m_countdownWidgetID))
            countdown->SetVisible(Visibility::Hidden);
    }

    constexpr Float2 imageSize{ 1920, 1080 };
    const XMFLOAT3 leftStartPos{ -imageSize.x * 0.5f, centerY, 0.0f };
    const XMFLOAT3 rightStartPos{ canvas.x + imageSize.x * 0.5f, centerY, 0.0f };

    auto* leftImage = CreateWidget<Image>(
        L"VS_Left",
        imageSize,
        leftStartPos,
        Float2{ 0.5f, 0.5f });
    auto* rightImage = CreateWidget<Image>(
        L"VS_Right",
        imageSize,
        rightStartPos,
        Float2{ 0.5f, 0.5f });

    if (leftImage)
    {
        leftImage->SetLayer(WidgetLayer::HUD);
        leftImage->ChangeTexture(L"../Assets/UI/WEAPON_SELECT/4_V.png");
        m_leftVsWidgetID = leftImage->GetID();
    }

    if (rightImage)
    {
        rightImage->SetLayer(WidgetLayer::HUD);
        rightImage->ChangeTexture(L"../Assets/UI/WEAPON_SELECT/4_S.png");
        m_rightVsWidgetID = rightImage->GetID();
    }

    m_vsElapsed = 0.0f;
    m_phase = VsPhase::VsSlide;
}

void CountdownScene::UpdateVsSlide(float dt)
{
    m_vsElapsed += dt;
    const float t = std::clamp(m_vsElapsed / kVsSlideDuration, 0.0f, 1.0f);

    auto canvas = m_uiManager->GetCanvasSize();
    const float centerY = canvas.y * 0.5f;
    const float centerX = canvas.x * 0.5f;
    constexpr float halfWidth = 256.0f;

    const float leftStartX = -halfWidth;
    const float rightStartX = canvas.x + halfWidth;
    const float leftEndX = centerX - halfWidth;
    const float rightEndX = centerX + halfWidth;

    if (auto* left = m_uiManager->FindWidget(m_leftVsWidgetID))
    {
        const float x = leftStartX + (leftEndX - leftStartX) * t;
        left->SetPos(XMFLOAT3{ x, centerY, 0.0f });
    }

    if (auto* right = m_uiManager->FindWidget(m_rightVsWidgetID))
    {
        const float x = rightStartX + (rightEndX - rightStartX) * t;
        right->SetPos(XMFLOAT3{ x, centerY, 0.0f });
    }

    if (t >= 1.0f)
        m_phase = VsPhase::Finished;
}

void CountdownScene::ClearVsWidgets()
{
    if (m_leftVsWidgetID != UINT_MAX)
    {
        m_uiManager->DestroyWidget(m_leftVsWidgetID);
        m_leftVsWidgetID = UINT_MAX;
    }

    if (m_rightVsWidgetID != UINT_MAX)
    {
        m_uiManager->DestroyWidget(m_rightVsWidgetID);
        m_rightVsWidgetID = UINT_MAX;
    }
}

void CountdownScene::OnEnter()
{
    m_phase = VsPhase::Countdown;
    m_lastNumber = -1;
    m_vsElapsed = 0.0f;
    m_roundStartRequested = false;
}

void CountdownScene::OnExit()
{
    // 나갈 때 UI 정리
    if (m_countdownWidgetID != UINT_MAX)
    {
        m_uiManager->DestroyWidget(m_countdownWidgetID);
        m_countdownWidgetID = UINT_MAX;
    }

    ClearVsWidgets();
}

void CountdownScene::Update(float dt)
{
    SceneBase::Update(dt);

    GameManager& gm = GameManager::Get();

    if (m_phase == VsPhase::Countdown)
    {
        if (gm.IsCountdownActive())
        {
            int num = gm.GetCountdownNumber();
            if (num > 0 && num != m_lastNumber)
            {
                m_lastNumber = num;
                ShowOrUpdateCountdownImage(num);
            }
        }
        else if (gm.IsCountdownFinished())
        {
            StartVsSlide();
        }
    }
    else if (m_phase == VsPhase::VsSlide)
    {
        UpdateVsSlide(dt);
    }
    else if (m_phase == VsPhase::Finished && !m_roundStartRequested)
    {
        m_roundStartRequested = true;
        gm.SetSceneState(CurrentSceneState::RoundStart);
    }
}

void CountdownScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void CountdownScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
