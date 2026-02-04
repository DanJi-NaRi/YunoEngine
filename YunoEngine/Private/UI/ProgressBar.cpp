#include "pch.h"
#include "ProgressBar.h"

#include "UIFactory.h"
#include "IInput.h"

#include "Gauge.h"

ProgressBar::ProgressBar(UIFactory& uiFactory) : Widget(uiFactory)
{
}

ProgressBar::~ProgressBar()
{
}

bool ProgressBar::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;

}

void ProgressBar::CreateChild()
{
    //m_pGauge = m_uiFactory;
    m_pGauge = m_uiFactory.CreateChild<Gauge>(m_name + L"_Gauge0", Float2(1083, 34), XMFLOAT3(0, 0, 0), UIDirection::Center, this);
    m_pGauge->SetFillDirection(FillDirection::BottomToTop);
}

bool ProgressBar::Start()
{
    return true;
}

bool ProgressBar::Update(float dTime)
{
    Widget::Update(dTime);
    GuageUpdate(dTime);

    return true;
}

bool ProgressBar::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    Widget::Submit(dTime);

    return true;
}

void ProgressBar::GuageUpdate(float dTime)
{
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_pGauge->AddValue(30 * dTime); }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_pGauge->SubValue(30 * dTime); }
}
