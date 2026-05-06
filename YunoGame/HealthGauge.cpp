#include "pch.h"
#include "HealthGauge.h"

#include "IInput.h"


HealthGauge::HealthGauge(UIFactory& uiFactory) : Gauge(uiFactory)
{
    m_value = 100.0f;
    m_fillDir = FillDirection::LeftToRight;
}

HealthGauge::~HealthGauge()
{
}

bool HealthGauge::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Gauge::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_gaugeMax = sizePx; // 처음 지정한 사이즈가 게이지의 최대치가 됨.
    return true;

}

bool HealthGauge::Start()
{
    Gauge::Start();

    return true;
}

bool HealthGauge::Update(float dTime)
{
    Gauge::Update(dTime);

    return true;
}

bool HealthGauge::Submit(float dTime)
{
    Gauge::Submit(dTime);
    return true;
}
