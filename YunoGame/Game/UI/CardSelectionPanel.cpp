#include "pch.h"
#include "CardSelectionPanel.h"

#include "Text.h"
#include "Button.h"
#include "Card.h"
#include "CardSlot.h"
#include "WeaponNameImage.h"
#include "PhaseStaminaBar.h"
#include "PhaseWeaponSelectButton.h"

#include "BattlePackets.h"
#include "GameManager.h"

#include "IInput.h"
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

    PieceType ToPieceTypeFromWeaponId(int weaponId)
    {
        if (weaponId > static_cast<int>(PieceType::None) && weaponId < static_cast<int>(PieceType::Count))
            return static_cast<PieceType>(weaponId);

        return PieceType::None;
    }

    PieceType ToPieceTypeFromAllowedMask(PieceMask mask)
    {
        if (mask == kAny || mask == kNone)
            return PieceType::None;

        if ((mask & PieceBit(PieceType::Blaster)) != 0) return PieceType::Blaster;
        if ((mask & PieceBit(PieceType::Chakram)) != 0) return PieceType::Chakram;
        if ((mask & PieceBit(PieceType::Breacher)) != 0) return PieceType::Breacher;
        if ((mask & PieceBit(PieceType::Scythe)) != 0) return PieceType::Scythe;
        if ((mask & PieceBit(PieceType::Impactor)) != 0) return PieceType::Impactor;
        if ((mask & PieceBit(PieceType::Cleaver)) != 0) return PieceType::Cleaver;
        return PieceType::None;
    }

    int ParseCardIndex(const std::wstring& cardName)
    {
        const size_t underscorePos = cardName.rfind(L"_");
        if (underscorePos == std::wstring::npos || underscorePos + 1 >= cardName.size())
            return 0;

        const std::wstring indexPart = cardName.substr(underscorePos + 1);
        if (indexPart.empty())
            return 0;

        for (wchar_t ch : indexPart)
        {
            if (ch < L'0' || ch > L'9')
                return 0;
        }

        return std::stoi(indexPart);
    }

    std::wstring BuildCardTexturePath(PieceType pieceType, int cardIndex, bool isSelected)
    {
        const std::wstring lowerName = ToLowerPieceName(pieceType);
        if (lowerName.empty() || cardIndex <= 0)
            return L"../Assets/UI/CARD/Card_back.png";

        if (isSelected)
        {
            return L"../Assets/UI/CARD_locked/card_" + lowerName + L"_locked_" + std::to_wstring(cardIndex) + L".png";
        }

        return L"../Assets/UI/CARD/card_" + lowerName + L"_" + std::to_wstring(cardIndex) + L".png";
    }
}

CardSelectionPanel::CardSelectionPanel(UIFactory& uiFactory) : PhasePanel(uiFactory)
{
    Clear();
}

CardSelectionPanel::~CardSelectionPanel()
{
    Clear();
}

void CardSelectionPanel::Clear()
{
}

bool CardSelectionPanel::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    PhasePanel::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_anchor = UIDirection::LeftTop;
    
    m_curPage = 0;
    m_curFilter = CardType::None;
    m_curSlot = 0;

    Backup();

    return true;
}

void CardSelectionPanel::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);

    // 웨폰 수직 이름 이미지
    m_pWeaponIMG = m_uiFactory.CreateChild<WeaponNameImage>(m_name + L"_WeaponName", Float2(56, 259), XMFLOAT3(-1456, -299, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);

    // 필터 버튼
    {
        m_pFilterButtons[0] = m_uiFactory.CreateChild<Button>(m_name + L"FillterBtn_All", Float2(296, 50), XMFLOAT3(-1328, -505, 0), UIDirection::Center, this);
        m_pFilterButtons[0]->SetVisible(Visibility::Hidden);
        m_pFilterButtons[0]->SetEventLMB([this]() {this->ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/tap_all_click.png"); m_curFilter = CardType::None; ViewCardPage(m_curSlot, 0); });

        m_pFilterButtons[1] = m_uiFactory.CreateChild<Button>(m_name + L"FillterBtn_Buff", Float2(165, 50), XMFLOAT3(-1070, -505, 0), UIDirection::Center, this);
        m_pFilterButtons[1]->SetVisible(Visibility::Hidden);
        m_pFilterButtons[1]->SetEventLMB([this]() {this->ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/tap_buff_click.png"); m_curFilter = CardType::Buff; ViewCardPage(m_curSlot, 0); });

        m_pFilterButtons[2] = m_uiFactory.CreateChild<Button>(m_name + L"FillterBtn_Move", Float2(165, 50), XMFLOAT3(-886, -505, 0), UIDirection::Center, this);
        m_pFilterButtons[2]->SetVisible(Visibility::Hidden);
        m_pFilterButtons[2]->SetEventLMB([this]() {this->ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/tap_move_click.png"); m_curFilter = CardType::Move; ViewCardPage(m_curSlot, 0); });
        
        m_pFilterButtons[3] = m_uiFactory.CreateChild<Button>(m_name + L"FillterBtn_Attack", Float2(165, 50), XMFLOAT3(-702, -505, 0), UIDirection::Center, this);
        m_pFilterButtons[3]->SetVisible(Visibility::Hidden);
        m_pFilterButtons[3]->SetEventLMB([this]() {this->ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/tap_attack_click.png"); m_curFilter = CardType::Attack; ViewCardPage(m_curSlot, 0); });
        
        m_pFilterButtons[4] = m_uiFactory.CreateChild<Button>(m_name + L"FillterBtn_Util", Float2(165, 50), XMFLOAT3(-518, -505, 0), UIDirection::Center, this);
        m_pFilterButtons[4]->SetVisible(Visibility::Hidden);
        m_pFilterButtons[4]->SetEventLMB([this]() {this->ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/tap_special_click.png"); m_curFilter = CardType::Utility; ViewCardPage(m_curSlot, 0); });
    }

    // 스태미나 바
    {
        m_pPhaseStaminaBars[0] = m_uiFactory.CreateChild<PhaseStaminaBar>(m_name + L"_PhaseSTABar_0", Float2(1083, 34), XMFLOAT3(-650, -450, 0), UIDirection::Center, this);
        m_pPhaseStaminaBars[0]->GetWeponSelectButton()->ChangeWeaponImage(m_player.weapons[0].weaponId);
        m_pPhaseStaminaBars[0]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(0, 0); this->m_pWeaponIMG->ChangeWeaponImage(m_player.weapons[0].weaponId); m_curSlot = 0; }); // 0번 슬롯 CardPage 0번으로 이동
        
        m_pPhaseStaminaBars[1] = m_uiFactory.CreateChild<PhaseStaminaBar>(m_name + L"_PhaseSTABar_1", Float2(1083, 34), XMFLOAT3(-650, -400, 0), UIDirection::Center, this);
        m_pPhaseStaminaBars[1]->GetWeponSelectButton()->ChangeWeaponImage(m_player.weapons[1].weaponId);
        m_pPhaseStaminaBars[1]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(1, 0); this->m_pWeaponIMG->ChangeWeaponImage(m_player.weapons[1].weaponId); m_curSlot = 1; }); // 0번 슬롯 CardPage 0번으로 이동
    }
    
    // 페이지 버튼
    {
        m_pPageText = m_uiFactory.CreateChild<Text>(m_name + L"_PageNum", Float2(74, 66), XMFLOAT3(-100, -302, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::Center, this);
        m_pPageText->SetText(std::to_wstring(m_curPage));
        m_pPageText->SetTextScale(XMFLOAT3(1.5f, 1.5f, 1.5f));
        m_pPageText->SetFont(FontID::Number);


        m_pPageUpButton = m_uiFactory.CreateChild<Button>(m_name + L"_PageUp", Float2(74, 66), XMFLOAT3(-100, -302, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);
        assert(m_pPageUpButton);
        m_pPageUpButton->SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseout.png", L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseover.png");
        m_pPageUpButton->SetEventLMB([this]() { this->PageUp(m_curSlot); });

        m_pPageDownButton = m_uiFactory.CreateChild<Button>(m_name + L"_PageDown", Float2(74, 66), XMFLOAT3(-100, -66, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);
        assert(m_pPageDownButton);
        m_pPageDownButton->MirrorScaleY();
        m_pPageDownButton->SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseout.png", L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseover.png");
        m_pPageDownButton->SetEventLMB([this]() { this->PageDown(m_curSlot); });
    }

    // 카드
    {
        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C0", Float2(205, 297), XMFLOAT3(-1382, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C1", Float2(205, 297), XMFLOAT3(-1169, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C2", Float2(205, 297), XMFLOAT3(-955, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C3", Float2(205, 297), XMFLOAT3(-740, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C4", Float2(205, 297), XMFLOAT3(-526, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C5", Float2(205, 297), XMFLOAT3(-312, -331, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        m_pCards.back()->SetLayer(WidgetLayer::Card);

        for (auto* card : m_pCards)
        {
            if (!card) continue;
            card->SetEventLMB([this, card]() {
                if (card->GetCardID() == 0) return;
                m_pSelectedCard = card;
                });
        }
    }



}

bool CardSelectionPanel::Start()
{
    PhasePanel::Start();

    ViewCardPage(0, 0); // 0번 슬롯, 0번 페이지

    m_pWeaponIMG->ChangeWeaponImage(m_player.weapons[0].weaponId);
    return true;
}

bool CardSelectionPanel::Update(float dTime)
{
    PhasePanel::Update(dTime);

    for (int i = 0; i < static_cast<int>(m_pPhaseStaminaBars.size()); ++i)
    {
        if (!m_pPhaseStaminaBars[i])
            continue;

        const int maxStamina = std::max(1, m_player.weapons[i].maxStamina);
        const float staminaRatio = static_cast<float>(m_player.weapons[i].stamina) / static_cast<float>(maxStamina);
        m_pPhaseStaminaBars[i]->SetStaminaValue(staminaRatio * 100.0f);
    }

    return true;
}

bool CardSelectionPanel::Submit(float dTime)
{
    PhasePanel::Submit(dTime);
    return false;
}

void CardSelectionPanel::UpdatePanel(const BattleResult& battleResult)
{
    
}
void CardSelectionPanel::UpdatePanel(const ObstacleResult& obstacleResult)
{
    
}

void CardSelectionPanel::ViewCardPage(int slot, int page)
{
    //const auto& cards = m_player.hands[slot].cards; // 카드 받기
    
    const auto cards = GetCardsByFilter(slot, m_curFilter);

    constexpr int CardSlotSize = 6; // 한 페이지 당 카드 갯수
    const int total = static_cast<int>(cards.size()); // 카드 총 사이즈

    const int viewCount = std::min(CardSlotSize, static_cast<int>(m_pCards.size()));

    if (total == 0)
    {
        m_curPage = 0;
        m_pPageText->SetText(std::to_wstring(m_curPage));
        for (int i = 0; i < viewCount; ++i)
        {
            m_pCards[i]->SetCardID(0);
            m_pCards[i]->SetDraggable(false);
            m_pCards[i]->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        }
        return;
    }


    // total==0이면 maxPage=-1 (페이지가 "없다"는 의미)
    const int maxPage = GetMaxPage(slot);

    page = std::clamp(page, 0, maxPage);
    m_curPage = page;
    m_pPageText->SetText(std::to_wstring(m_curPage));

    const int startIdx = page * CardSlotSize;

    
    for (int i = 0; i < viewCount; ++i)
    {
        const int idx = startIdx + i;

        if (idx < total)
        {
            const auto dataID = cards[idx].dataID;
            const auto runtimeID = cards[idx].runtimeID;
            m_pCards[i]->SetCardID(runtimeID);
            m_pCards[i]->SetSlotID(slot);
            const CardData cardData = m_cardManager.GetCardData(static_cast<int>(dataID));

            PieceType pieceType = PieceType::None;
            if (slot >= 0 && slot < static_cast<int>(m_player.weapons.size()))
            {
                pieceType = ToPieceTypeFromWeaponId(m_player.weapons[slot].weaponId);
            }

            if (pieceType == PieceType::None)
            {
                pieceType = ToPieceTypeFromAllowedMask(cardData.m_allowedUnits);
            }

            const int cardIndex = ParseCardIndex(cardData.m_name);
            const bool isSelectedInConfirmSlots = m_gameManager.IsRuntimeCardInConfirmSlots(runtimeID);
            m_pCards[i]->SetDraggable(!isSelectedInConfirmSlots);
            m_pCards[i]->ChangeTexture(BuildCardTexturePath(pieceType, cardIndex, isSelectedInConfirmSlots));
        }
        else
        {
            // 남는 슬롯은 카드 뒷면으로 채움
            m_pCards[i]->SetCardID(0);    // 아이디 등록
            m_pCards[i]->SetSlotID(slot); // 슬롯 순번 등록
            m_pCards[i]->SetDraggable(false);
            m_pCards[i]->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        }
    }
}


std::vector<ClientCardInfo> CardSelectionPanel::GetCardsByFilter(int slot, const CardType filter)
{
    //const auto& cards = m_player.hands[slot].cards; // 카드 받기
    const std::vector<ClientCardInfo>& cards = m_player.hands[slot].cards; // 카드 받기
    std::vector<ClientCardInfo> filterCards;

    if (filter == CardType::None || filter == CardType::Count) return cards;

    for (auto& card : cards) { // 필터에 맞는 카드만 주기
        CardType type = m_cardManager.GetCardData(m_gameManager.GetCardDataID(card.runtimeID)).m_type;
        if (type == filter) filterCards.push_back(card);
    }

    return filterCards;
}

void CardSelectionPanel::PageUp(int slot)
{
    const int maxPage = GetMaxPage(slot);
    if (maxPage < 0) { m_curPage = 0; ViewCardPage(slot, 0); return; }

    m_curPage = std::clamp(m_curPage - 1, 0, maxPage);
    ViewCardPage(slot, m_curPage);
}

void CardSelectionPanel::PageDown(int slot)
{
    const int maxPage = GetMaxPage(slot);
    if (maxPage < 0) { m_curPage = 0; ViewCardPage(slot, 0); return; }

    m_curPage = std::clamp(m_curPage + 1, 0, maxPage);
    ViewCardPage(slot, m_curPage);
}

void CardSelectionPanel::RefreshCardVisualState()
{
    ViewCardPage(m_curSlot, m_curPage);
}

const int CardSelectionPanel::GetMaxPage(int slot)
{
    if (slot < 0 || slot >= static_cast<int>(m_player.hands.size()))
        return -1;

    const auto cards = GetCardsByFilter(slot, m_curFilter);

    constexpr int CardSlotSize = 6; // 한 페이지 당 카드 갯수
    const int total = static_cast<int>(cards.size()); // 카드 총 사이즈
    // total==0이면 maxPage=-1 (페이지가 "없다"는 의미)
    return (total == 0) ? -1 : (total - 1) / CardSlotSize;
}
