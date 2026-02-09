#pragma once
#include "Image.h"

class MinimapTile : public Image
{
public:
    MinimapTile(UIFactory& uiFactory);
    virtual ~MinimapTile();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::MinimapTile; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/map_cube_non.png"); };
protected:
    bool m_isPlayerTile;
};
