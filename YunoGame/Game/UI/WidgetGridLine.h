#pragma once
#include "Widget.h"

class WidgetGridLine : public Widget
{
public:
    // 생성자는 (UIFactory& uiFactory) 인자를 기본으로
    WidgetGridLine(UIFactory& uiFactory);
    virtual ~WidgetGridLine();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;             // 위젯 생성 동시에 바로 다음 차례에 자식 위젯을 생성해야 한다면 (자식 생성 공간)
    bool Start() override;                   // 생성 이후, 시작 지점 (자식 객체를 생성한다면 여기에)
    bool Update(float dTime = 0) override;   // UIManager가 전체 순회 업데이트 (Logic)
    bool Submit(float dTime = 0) override;
    void Clear();

    // 무조건 수정해주기. Class 없으면 WidgetClass 따라가서 추가해주기.
    virtual WidgetType GetWidgetType() override { return WidgetType::Debug; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::WidgetGridLine; }

    bool CreateMesh() override; // 메시 생성 (한 번만)
    virtual bool CreateMaterial() override { return Widget::CreateMaterialDebug(L""); };    // 머테리얼 생성 // 알베도 없음
protected:
    // 라인 그리기용 버텍스
    std::vector<VERTEX_Pos> m_lineVtx;
};
