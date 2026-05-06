#pragma once
#include "Widget.h"
class Gauge;
enum class FillDirection;

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class ProgressBar : public Widget {
public:
    explicit ProgressBar(UIFactory& uiFactory);
    virtual ~ProgressBar();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual void GaugeUpdate(float dTime = 0);

    virtual WidgetType GetWidgetType() override { return WidgetType::ProgressBar; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::ProgressBar; }

    void SetFillDirect(FillDirection dir);
    void SetGaugeValue(float v);

protected:
    Gauge* m_pGauge;

private:
    bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/Bar_base.png"); }  // 머테리얼 생성 (한 번만)
};
