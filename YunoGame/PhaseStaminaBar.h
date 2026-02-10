#pragma once
#include "StaminaBar.h"
class StaminaGauge;
class PhaseWeaponSelectButton;

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class PhaseStaminaBar : public StaminaBar {
public:
    explicit PhaseStaminaBar(UIFactory& uiFactory);
    virtual ~PhaseStaminaBar();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual void GuageUpdate(float dTime = 0);

    virtual WidgetType GetWidgetType() override { return WidgetType::ProgressBar; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::PhaseStaminaBar; }

    PhaseWeaponSelectButton* GetWeponSelectButton() { if (m_pWeaponButton) return m_pWeaponButton; else return nullptr; }
private:
    bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/Bar_base.png"); }  // 머테리얼 생성 (한 번만)

protected:
    PhaseWeaponSelectButton* m_pWeaponButton = nullptr;
};
