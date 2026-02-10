#pragma once
#include "Image.h"

enum class PieceType : uint8_t;

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


    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::PhasePanel; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Test/Weapon_Noting.png"); };    // 머테리얼 생성 (한 번만)

    void ChangeWeaponImage();
    void ClearSlot();

    void SetPieceType(int type); 
    void SetPieceType(PieceType type);

protected:
    int m_weaponID;
};

