#pragma once
#include "Image.h"

class TextureImage : public Image
{
public:
    TextureImage(UIFactory& uiFactory);
    virtual ~TextureImage();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Image; }

    void SetTexturePath(const std::wstring& texturePath) { m_texturePath = texturePath; }
    const std::wstring& GetTexturePath() const { return m_texturePath; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(m_texturePath); }

private:
    std::wstring m_texturePath = L"../Assets/Textures/black.png";
};
