#pragma once
#include "Image.h"


class CountdownImage : public Image
{
public:
    CountdownImage(UIFactory& uiFactory);
    virtual ~CountdownImage();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::TitleImage; }

    bool ChangeMaterial(int num);
protected:
    virtual bool CreateMaterial() override
    {
        Widget::CreateMaterial(L"../Assets/UI/WEAPON_SELECT/3.png");

        m_size.x = m_pTextures->GetTextureWH(m_Albedo).first/2;
        m_size.y = m_pTextures->GetTextureWH(m_Albedo).second/2;
        return true;
    };    // 머테리얼 생성 (한 번만)

private:
    bool m_userJoined = false;
};
