#pragma once
#include "Gauge.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class HealthGauge : public Gauge {
public:
    explicit HealthGauge(UIFactory& uiFactory);
    virtual ~HealthGauge();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual WidgetType GetWidgetType() override { return WidgetType::Gauge; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::HealthGauge; }

private:
    bool CreateMaterial() override { return Gauge::CreateMaterial(L"../Assets/UI/Widget/PlayerHUD/Bar_HP.png"); }  // 머테리얼 생성 (한 번만)

};
