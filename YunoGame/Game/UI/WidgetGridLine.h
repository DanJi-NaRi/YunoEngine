#pragma once
#include "Widget.h"
#include "GridFactory.h"

class WidgetGridLine : public Widget
{
public:
    WidgetGridLine(UIFactory& ui)
        : Widget(ui) {}
    virtual ~WidgetGridLine();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
private:
    bool CreateMesh();      // 메시 생성 (한 번만)
    bool CreateMaterial();  // 머테리얼 생성 (한 번만)

private:
    // 라인 그리기용 버텍스
    std::vector<VERTEX_Pos> m_lineVtx;
};

