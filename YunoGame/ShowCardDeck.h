#pragma once

#include <array>
#include <string>

#include "SceneBase.h"
#include "Button.h"
#include "TextureImage.h"
#include "CardData.h"

namespace
{
    inline std::wstring ToLowerPieceName(PieceType pieceType)
    {
        switch (pieceType)
        {
        case PieceType::Blaster: return L"blaster";
        case PieceType::Breacher: return L"breacher";
        case PieceType::Impactor: return L"impactor";
        case PieceType::Chakram: return L"chakram";
        case PieceType::Scythe: return L"scythe";
        case PieceType::Cleaver: return L"cleaver";
        default: return L"";
        }
    }

    inline std::wstring ToUpperPieceName(PieceType pieceType)
    {
        switch (pieceType)
        {
        case PieceType::Blaster: return L"BLASTER";
        case PieceType::Breacher: return L"BREACHER";
        case PieceType::Impactor: return L"IMPACTOR";
        case PieceType::Chakram: return L"CHAKRAM";
        case PieceType::Scythe: return L"SCYTHE";
        case PieceType::Cleaver: return L"CLEAVER";
        default: return L"";
        }
    }
}

class ShowCardButton final : public Button
{
public:
    explicit ShowCardButton(UIFactory& uiFactory) : Button(uiFactory) {}

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override
    {
        Button::Create(name, id, sizePx, vPos, rotZ, vScale);
        m_bindkey = 0;
        return true;
    }

    bool IdleEvent() override
    {
        if (m_pTooltipImage)
            m_pTooltipImage->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
        return true;
    }

    bool HoveredEvent() override
    {
        if (m_pTooltipImage)
            m_pTooltipImage->SetScale(XMFLOAT3(1.f, 1.f, 1.f));
        return true;
    }

    bool LMBPressedEvent() override { return true; }

    void SetTooltipImage(TextureImage* tooltipImage) { m_pTooltipImage = tooltipImage; }

private:
    bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/CARD/Card_back.png"); }

private:
    TextureImage* m_pTooltipImage = nullptr;
};

class ShowCard
{
public:
    bool Create(SceneBase& scene, const std::wstring& name, XMFLOAT3 position, const std::wstring& texturePath, bool enableTooltip);


    void SetCardTexture(const std::wstring& texturePath);


    void SetTooltipTexture(const std::wstring& texturePath);


private:
    ShowCardButton* m_pCardButton = nullptr;
    TextureImage* m_pTooltipImage = nullptr;
};

class ShowCardDeck
{
public:
    bool Create(SceneBase& scene, float centerX, float startY);

    void SetWeaponCards(PieceType pieceType);


private:
    std::array<ShowCard, 16> m_cards;
};
