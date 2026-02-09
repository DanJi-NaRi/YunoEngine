#pragma once

#include <array>

#include "Image.h"
#include "Button.h"
#include "TextureImage.h"
#include "CardData.h"

class ShowCardButton final : public Button
{
public:
    explicit ShowCardButton(UIFactory& uiFactory);

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;

    bool IdleEvent() override;
    bool HoveredEvent() override;
    bool LMBPressedEvent() override;

    void SetTooltipImage(TextureImage* tooltipImage);
    void SetTooltipTexturePath(const std::wstring& tooltipTexturePath);
    void SetTooltipEnabled(bool isEnabled);

private:
    bool CreateMaterial() override;

private:
    TextureImage* m_pTooltipImage = nullptr;
    std::wstring m_tooltipTexturePath;
    bool m_isTooltipEnabled = false;
};

class ShowCardDeck final : public Image
{
public:
    explicit ShowCardDeck(UIFactory& uiFactory);

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    WidgetType GetWidgetType() override { return WidgetType::Image; }
    WidgetClass GetWidgetClass() override { return WidgetClass::Image; }

    void SetWeaponCards(PieceType pieceType);

private:
    void BuildCards();
    bool CreateMaterial() override;

private:
    bool m_isBuilt = false;
    std::array<ShowCardButton*, 16> m_cardButtons{};
    std::array<TextureImage*, 16> m_pTooltipImages{};
    TextureImage* m_pTooltipImage = nullptr;
};
