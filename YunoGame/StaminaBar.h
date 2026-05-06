#pragma once
#include "ProgressBar.h"
class StaminaGauge;

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class StaminaBar : public ProgressBar {
public:
    explicit StaminaBar(UIFactory& uiFactory);
    virtual ~StaminaBar();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual void GuageUpdate(float dTime = 0);
    void SetStaminaValue(float value);


    virtual WidgetType GetWidgetType() override { return WidgetType::ProgressBar; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::StaminaBar; }

private:
    bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/Bar_base.png"); }  // 머테리얼 생성 (한 번만)
};
