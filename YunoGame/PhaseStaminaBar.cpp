#include "pch.h"
#include "PhaseStaminaBar.h"

#include "UIFactory.h"
#include "IInput.h"

#include "StaminaBar.h"
#include "StaminaGauge.h"

#include "PhaseWeaponSelectButton.h"

PhaseStaminaBar::PhaseStaminaBar(UIFactory& uiFactory) : StaminaBar(uiFactory)
{
}

PhaseStaminaBar::~PhaseStaminaBar()
{
}

bool PhaseStaminaBar::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    StaminaBar::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;

}

void PhaseStaminaBar::CreateChild()
{
    //m_pGauge = m_uiFactory;
    m_pGauge = m_uiFactory.CreateChild<StaminaGauge>(m_name + L"_Gauge0", Float2(1077, 28), XMFLOAT3(0, 0, 0), UIDirection::Center, this);
    m_pGauge->SetFillDirection(FillDirection::LeftToRight);
    m_pGauge->ChangeTexture(L"../Assets/UI/PLAY/Bar_STA_mirror.png"); // 목업 기준 사선이 오른쪽인 반전 텍스처 사용

#if 0
    m_pWeaponButton = m_uiFactory.CreateChild<PhaseWeaponSelectButton>(m_name + L"_Button", Float2(230, 72), XMFLOAT3(-762, -35, 0), UIDirection::LeftTop, this); // 텍스처 투명여백(8px) 감안, 게이지 왼쪽 끝에 밀착
#endif
}

bool PhaseStaminaBar::Start()
{
    return true;
}

bool PhaseStaminaBar::Update(float dTime)
{
    StaminaBar::Update(dTime);
    GuageUpdate(dTime);

    return true;
}

bool PhaseStaminaBar::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    StaminaBar::Submit(dTime);

    return true;
}

void PhaseStaminaBar::GuageUpdate(float dTime)
{
}
