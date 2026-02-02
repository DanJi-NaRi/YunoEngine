#pragma once
#include "Widget.h"

class Text : public Widget {
public:
    explicit Text(UIFactory& uiFactory);
    ~Text() override;

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Start() override;
    bool UpdateTransform(float dTime = 0) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    void SetText(const std::wstring& text) { m_desc.text = text; }
    void SetFont(FontID fontId) { m_desc.fontId = fontId; }
    void SetColor(const XMFLOAT4& color) { m_desc.color = color; }
    void SetTextScale(const XMFLOAT3& scale) { m_vScale = scale; }
    void SetTextOffset(const XMFLOAT2& offset) { m_textOffset = offset; }

    const std::wstring& GetText() const { return m_desc.text; }
    FontID GetFont() const { return m_desc.fontId; }
    XMFLOAT4 GetColor() const { return m_desc.color; }
    XMFLOAT2 GetTextScale() const { return m_desc.scale; }
    XMFLOAT2 GetTextOffset() const { return m_textOffset; }

    virtual WidgetType GetWidgetType() override { return WidgetType::Text; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Text; }

private:
    bool CreateMaterial() override;

    XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float m_textScale = 1.0f;
    XMFLOAT2 m_textOffset = { 0.0f, 0.0f };
    TextDrawDesc m_desc;
};
