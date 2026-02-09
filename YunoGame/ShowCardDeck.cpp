#include "pch.h"

#include "ShowCardDeck.h"
#include "UIFactory.h"

namespace
{
    std::wstring ToLowerPieceName(PieceType pieceType)
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

    std::wstring ToUpperPieceName(PieceType pieceType)
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

ShowCardButton::ShowCardButton(UIFactory& uiFactory) : Button(uiFactory)
{
}

bool ShowCardButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    sizePx /= 2;
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);
    m_bindkey = 0;
    return true;
}

bool ShowCardButton::IdleEvent()
{
    if (m_pTooltipImage)
        m_pTooltipImage->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
    return true;
}

bool ShowCardButton::HoveredEvent()
{
    if (m_pTooltipImage)
    {
        if (!m_tooltipTexturePath.empty())
            m_pTooltipImage->ChangeTexture(m_tooltipTexturePath);

        m_pTooltipImage->SetScale(XMFLOAT3(1.f, 1.f, 1.f));
    }
    return true;
}

bool ShowCardButton::LMBPressedEvent()
{
    return true;
}

void ShowCardButton::SetTooltipImage(TextureImage* tooltipImage)
{
    m_pTooltipImage = tooltipImage;
}

void ShowCardButton::SetTooltipTexturePath(const std::wstring& tooltipTexturePath)
{
    m_tooltipTexturePath = tooltipTexturePath;

    if (m_pTooltipImage && !m_tooltipTexturePath.empty())
        m_pTooltipImage->ChangeTexture(m_tooltipTexturePath);

}

bool ShowCardButton::CreateMaterial()
{
    return Widget::CreateMaterial(L"../Assets/UI/CARD/Card_back.png");
}

ShowCardDeck::ShowCardDeck(UIFactory& uiFactory) : Image(uiFactory)
{
}

bool ShowCardDeck::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);
    BuildCards();
    SetWeaponCards(PieceType::None);
    return true;
}

bool ShowCardDeck::Update(float dTime)
{
    Image::Update(dTime);
    return true;
}

bool ShowCardDeck::Submit(float dTime)
{
    Image::Submit(dTime);
    return true;
}

void ShowCardDeck::SetWeaponCards(PieceType pieceType)
{
    const std::wstring lowerName = ToLowerPieceName(pieceType);
    if (lowerName.empty())
        return;


    for (int i = 0; i < static_cast<int>(m_cardButtons.size()); ++i)
    {
        if (!m_cardButtons[i])
            continue;

        //m_cardButtons[i]->ChangeTexture(L"../Assets/UI/CARD/card_" + lowerName + L"_" + std::to_wstring(i + 1) + L".png"); 
        m_cardButtons[i]->ChangeTexture(L"../Assets/UI/CARD/card_" + lowerName + L"_" + std::to_wstring(i + 1) + L".png");
        m_cardButtons[i]->SetTooltipTexturePath(L"../Assets/UI/TOOLTIP/tooltip_" + lowerName + L"_" + std::to_wstring(i + 1) + L".png");
    }
}

void ShowCardDeck::BuildCards()
{
    if (m_isBuilt)
        return;

    static constexpr int kSlotOfCard[16] =
    {
        0,  // 1  -> slot0
        1,  // 2  -> slot1
        2,  // 3  -> slot2
        8,  // 4  -> slot8
        9,  // 5  -> slot9
        10, // 6  -> slot10
        3,  // 7  -> slot3
        4,  // 8  -> slot4
        5,  // 9  -> slot5
        6,  // 10 -> slot6
        7,  // 11 -> slot7
        11, // 12 -> slot11
        12, // 13 -> slot12
        13, // 14 -> slot13
        14, // 15 -> slot14
        15  // 16 -> slot15
    };


    const float centerX = m_uiFactory.GetClientWidth() / 2.f;
    constexpr float startY = 313.0f;
    constexpr float kCardGap = 102.5f;
    constexpr float kStartOffsetX = -4.0f * kCardGap;
    constexpr float kStartOffsetY = 10.0f;

    for (int i = 0; i < static_cast<int>(m_cardButtons.size()); ++i)
    {
        const int slot = kSlotOfCard[i];
        const int row = slot / 8;
        const int col = slot % 8;

        XMFLOAT3 cardPos(centerX + kStartOffsetX + (col * kCardGap), startY + (row * 148.5f + row * kStartOffsetY), 0.f);
        if (col > 2)
            cardPos.x += 38.0f;

        m_cardButtons[i] = m_uiFactory.CreateChild<ShowCardButton>(L"ShowCard_" + std::to_wstring(i + 1), Float2(205, 297), cardPos, UIDirection::LeftTop, this);
        if (!m_cardButtons[i])
            continue;

        //m_cardButtons[i]->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");

        const float tooltipOffsetX = 80.f;
        const XMFLOAT3 tooltipPos(cardPos.x + tooltipOffsetX, cardPos.y, -1.f);

        auto* tooltipImage = m_uiFactory.CreateChild<TextureImage>(
            L"tooltip_" + std::to_wstring(i + 1), Float2(360, 462), tooltipPos, UIDirection::LeftTop, this);

        if (tooltipImage)
        {
            tooltipImage->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
            m_cardButtons[i]->SetTooltipImage(tooltipImage);
        }

        m_cardButtons[i]->SetTooltipTexturePath(L"../Assets/UI/TOOLTIP/tooltip_blaster_" + std::to_wstring(i + 1) + L".png");
    }

    m_isBuilt = true;
}



bool ShowCardDeck::CreateMaterial()
{
    return Widget::CreateMaterial(L"../Assets/Textures/black.png");
}
