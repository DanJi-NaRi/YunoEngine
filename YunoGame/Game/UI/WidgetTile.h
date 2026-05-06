#pragma once
#include "Widget.h"

struct C3
{
    float r, g, b;
};

class WidgetTile : public Widget
{
public:
    explicit WidgetTile(UIFactory& uiFactory);
    virtual ~WidgetTile();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

private:
    void FlashColor(C3 color, int count, float diff, float speed);

private:
    C3 m_maskColor{ 1, 1, 1 };
    float m_diff = 0;
    float m_FxSpeed = 0.f;
    int m_count = 0;

    bool isDone = false;
};
