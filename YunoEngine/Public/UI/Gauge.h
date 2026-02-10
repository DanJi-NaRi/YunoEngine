#pragma once
#include "Widget.h"

enum class FillDirection : int { // 채워지는 방향
    LeftToRight, // 왼쪽  -> 오른쪽
    RightToLeft, // 오른쪽 -> 왼쪽
    BottomToTop, // 아래  -> 위쪽
    TopToBottom, // 위쪽  -> 아래
};

//struct GaugeInfo
//{
//    float value; // 0.0f ~ 100.0f
//    float gaugeMax; // 최대 게이지 사이즈
//};

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class Gauge : public Widget {
public:
    explicit Gauge(UIFactory& uiFactory);
    virtual ~Gauge();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual void GuageUpdate(float dTime = 0);

    //////////////////////////////
    // Gauge 전용 함수들
    //////////////////////////////
    
    float& GetValue() { return m_value; }
    float SetValue(float value) { m_value = value; return m_value; }
    float AddValue(float value) { m_value += value; return m_value; }
    float SubValue(float value) { m_value -= value; return m_value; }

    void SetFillDirection(FillDirection fillDir);
    void SetGaugeMax(Float2 max) { m_gaugeMax = max; } // 게이지 최대치 재할당

    virtual WidgetType GetWidgetType() override { return WidgetType::Gauge; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Gauge; }

    bool IsFillHorizontal() const;
    bool IsFillVertical() const;

#ifdef _DEBUG
    virtual void Serialize() override;
#endif

protected:
    float m_value; // 0.0f ~ 100.0f 백분율
    Float2 m_gaugeMax; // 최대 게이지 사이즈 = (프로그래스 바 사이즈 - margin)
    FillDirection m_fillDir; // 채워지는 방향

    virtual bool CreateMaterial() override { return Gauge::CreateMaterial(L"../Assets/UI/PLAY/Bar_HP.png"); }  // 머테리얼 생성 (한 번만)
    virtual bool CreateMaterial(std::wstring path, MaterialDesc* pDesc = nullptr) override;
};
