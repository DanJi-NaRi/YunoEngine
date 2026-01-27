#pragma once
#pragma once
#include "Image.h"


class TitleImage : public Image
{
public:
    TitleImage(UIManager* uiManager);
    virtual ~TitleImage();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::TitleImage; }

    virtual bool CreateMaterial() override 
    { 
       Widget::CreateMaterial(L"../Assets/UI/TITLE/Background.png"); 

       m_width = m_pTextures->GetTextureWH(m_Albedo).first;
       m_height = m_pTextures->GetTextureWH(m_Albedo).second;
       return true;
    };    // 머테리얼 생성 (한 번만)


    bool ChangeMaterial(int num);

protected:
    bool m_userJoined = false;
};
