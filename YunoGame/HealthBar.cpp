#include "pch.h"
#include "HealthBar.h"

#include "UIFactory.h"
#include "IInput.h"

#include "HealthGauge.h"

HealthBar::HealthBar(UIFactory& uiFactory) : ProgressBar(uiFactory)
{
}

HealthBar::~HealthBar()
{
}

bool HealthBar::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    ProgressBar::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;

}

void HealthBar::CreateChild()
{
    //m_pGauge = m_uiFactory;
    m_pGauge = m_uiFactory.CreateChild<HealthGauge>(m_name + L"_Gauge0", Float2(1077, 28), XMFLOAT3(0, 0.6f, 0), UIDirection::Center, this);
    m_pGauge->SetFillDirection(FillDirection::LeftToRight);
}

bool HealthBar::Start()
{
    return true;
}

bool HealthBar::Update(float dTime)
{
    ProgressBar::Update(dTime);
    GuageUpdate(dTime);

    return true;
}

bool HealthBar::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    ProgressBar::Submit(dTime);

    return true;
}

void HealthBar::GuageUpdate(float dTime)
{
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_pGauge->AddValue(30 * dTime); }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_pGauge->SubValue(30 * dTime); }
}
