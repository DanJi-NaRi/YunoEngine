#include "pch.h"
#include "StaminaBar.h"

#include "UIFactory.h"
#include "IInput.h"

#include "StaminaGauge.h"

StaminaBar::StaminaBar(UIFactory& uiFactory) : ProgressBar(uiFactory)
{
}

StaminaBar::~StaminaBar()
{
}

bool StaminaBar::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    ProgressBar::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;

}

void StaminaBar::CreateChild()
{
    //m_pGauge = m_uiFactory;
    m_pGauge = m_uiFactory.CreateChild<StaminaGauge>(m_name + L"_Gauge0", Float2(538, 14), XMFLOAT3(0, 0.6f, 0), UIDirection::Center, this);
    m_pGauge->SetFillDirection(FillDirection::LeftToRight);
}

bool StaminaBar::Start()
{
    return true;
}

bool StaminaBar::Update(float dTime)
{
    ProgressBar::Update(dTime);
    GuageUpdate(dTime);

    return true;
}

bool StaminaBar::Submit(float dTime)
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

void StaminaBar::GuageUpdate(float dTime)
{
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_pGauge->AddValue(30 * dTime); }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_pGauge->SubValue(30 * dTime); }
}

void StaminaBar::SetStaminaValue(float value)
{
    if (!m_pGauge)
        return;

    m_pGauge->SetValue(value);
}
