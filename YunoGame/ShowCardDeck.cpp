#include "pch.h"

#include "ShowCardDeck.h"



bool ShowCard::Create(SceneBase& scene, const std::wstring& name, XMFLOAT3 position, const std::wstring& texturePath, bool enableTooltip)
{
    m_pCardButton = scene.CreateWidget<ShowCardButton>(name + L"_Btn", Float2(115, 161), position, UIDirection::Center);
    if (!m_pCardButton)
        return false;

    m_pCardButton->ChangeTexture(texturePath);

    if (enableTooltip)
    {
        m_pTooltipImage = scene.CreateWidget<TextureImage>(name + L"_Tooltip", L"../Assets/UI/WEAPON_SELECT/Info_BLASTER_mouseover.png", XMFLOAT3(position.x, position.y - 190.f, 0.f), UIDirection::Center);
        if (m_pTooltipImage)
        {
            m_pTooltipImage->SetScale(XMFLOAT3(0.f, 0.f, 1.f));
            m_pCardButton->SetTooltipImage(m_pTooltipImage);
        }
    }

    return true;
}

void ShowCard::SetCardTexture(const std::wstring& texturePath)
{
    if (m_pCardButton)
        m_pCardButton->ChangeTexture(texturePath);
}

void ShowCard::SetTooltipTexture(const std::wstring& texturePath)
{
    if (m_pTooltipImage)
        m_pTooltipImage->ChangeTexture(texturePath);
}





// ----------------------ShowCardDeck----------------------

bool ShowCardDeck::Create(SceneBase& scene, float centerX, float startY)
{
    constexpr float kCardGap = 118.f;
    constexpr float kStartOffsetX = -3.5f * kCardGap;

    for (int i = 0; i < static_cast<int>(m_cards.size()); ++i)
    {
        const int row = i / 8;
        const int col = i % 8;

        const XMFLOAT3 cardPos(centerX + kStartOffsetX + (col * kCardGap), startY + (row * 176.f), 0.f);
        const bool isFirstCard = (i == 0);

        if (!m_cards[i].Create(scene, L"ShowCard_" + std::to_wstring(i + 1), cardPos, L"../Assets/UI/CARD/Card_back.png", isFirstCard))
            return false;
    }

    return true;
}

void ShowCardDeck::SetWeaponCards(PieceType pieceType)
{
    const std::wstring lowerName = ToLowerPieceName(pieceType);
    const std::wstring upperName = ToUpperPieceName(pieceType);
    if (lowerName.empty())
        return;

    for (int i = 0; i < static_cast<int>(m_cards.size()); ++i)
    {
        const std::wstring path = L"../Assets/UI/CARD/card_" + lowerName + L"_" + std::to_wstring(i + 1) + L".png";
        m_cards[i].SetCardTexture(path);
    }

    m_cards[0].SetTooltipTexture(L"../Assets/UI/WEAPON_SELECT/Info_" + upperName + L"_mouseover.png");
}
