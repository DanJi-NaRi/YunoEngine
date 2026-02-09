#include "pch.h"

#include "CountdownScene.h"
#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "CountdownImage.h"
#include "Emoji.h"
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
        //std::cout << "A\n" << m_countdownWidgetID;
    }
    else
    {
        img = static_cast<CountdownImage*>(
            m_uiManager->FindWidget(m_countdownWidgetID)
            );
        //std::cout << "B\n";
    }        

    img->ChangeMaterial(num); // 3, 2, 1
}

void CountdownScene::OnEnter()
{
    // 필요하면 입력 막기
}

void CountdownScene::OnExit()
{
    // 나갈 때 UI 정리
    if (m_countdownWidgetID != 0)
    {
        m_uiManager->DestroyWidget(m_countdownWidgetID);
        m_countdownWidgetID = 0;
    }
}

void CountdownScene::Update(float dt)
{
    SceneBase::Update(dt);

    GameManager& gm = GameManager::Get();

    if (gm.IsCountdownActive())
    {
        int num = gm.GetCountdownNumber();
        if (num > 0 && num != m_lastNumber)
        {
            m_lastNumber = num;
            ShowOrUpdateCountdownImage(num);
        }
        else // 카운트 다운 끝난 경우 
        {
            CountdownImage* img = nullptr;
            img = static_cast<CountdownImage*>(
                m_uiManager->FindWidget(m_countdownWidgetID)
                );

            img->ChangeMaterial(num);
        }
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
