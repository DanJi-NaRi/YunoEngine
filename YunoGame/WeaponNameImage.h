#pragma once
#include "Image.h"


class WeaponNameImage : public Image
{
public:
    WeaponNameImage(UIFactory& uiFactory);
    virtual ~WeaponNameImage();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    void ClearSlot();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardTable; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Test/Weapon_Noting.png"); };    // 머테리얼 생성 (한 번만)
protected:
    int m_weaponID = 0;
};

