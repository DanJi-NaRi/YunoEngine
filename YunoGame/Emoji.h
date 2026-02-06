#pragma once
#include "Image.h"

class Emoji : public Image
{
public:
    Emoji(UIFactory& uiFactory);
    virtual ~Emoji();

    // Image와 동일한 Create 사용
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Emoji; }

    bool ChangeMaterial(int num);
protected:
    virtual bool CreateMaterial() override
    {
        Widget::CreateMaterial(
            L"../Assets/UI/PLAY/Imoji_EZ_mouseover.png"
        );

        m_size.x = m_pTextures->GetTextureWH(m_Albedo).first;
        m_size.y = m_pTextures->GetTextureWH(m_Albedo).second;
        return true;
    }; // 머테리얼 생성 (한 번만)

private:
    uint8_t m_emoteId = 0;
    bool m_userJoined = false;
};
