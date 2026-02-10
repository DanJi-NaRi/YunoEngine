#include "pch.h"
#include "CardSelectionPanel.h"

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
    m_curSlot = 0;

    Backup();

    return true;
}

void CardSelectionPanel::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);

    // 웨폰 수직 이름 이미지
    m_pWeaponIMG = m_uiFactory.CreateChild<WeaponNameImage>(m_name + L"_WeaponName", Float2(56, 259), XMFLOAT3(-1400, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);

    // 스태미나 바
    {

        m_pPhaseStaminaBars[0] = m_uiFactory.CreateChild<PhaseStaminaBar>(m_name + L"_PhaseSTABar_0", Float2(1083, 34), XMFLOAT3(-650, -450, 0), UIDirection::Center, this);
        m_pPhaseStaminaBars[0]->GetWeponSelectButton()->ChangeWeaponImage(m_player.weapons[0].weaponId);
        m_pPhaseStaminaBars[0]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(0, 0); this->m_pWeaponIMG->ChangeWeaponImage(m_player.weapons[0].weaponId); m_curSlot = 0; }); // 0번 슬롯 CardPage 0번으로 이동
        //m_pPhaseStaminaBars[0]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(0, 0); m_curSlot = 0; }); // 0번 슬롯 CardPage 0번으로 이동
        
        m_pPhaseStaminaBars[1] = m_uiFactory.CreateChild<PhaseStaminaBar>(m_name + L"_PhaseSTABar_1", Float2(1083, 34), XMFLOAT3(-650, -400, 0), UIDirection::Center, this);
        m_pPhaseStaminaBars[1]->GetWeponSelectButton()->ChangeWeaponImage(m_player.weapons[1].weaponId);
        m_pPhaseStaminaBars[1]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(1, 0); this->m_pWeaponIMG->ChangeWeaponImage(m_player.weapons[1].weaponId); m_curSlot = 1; }); // 0번 슬롯 CardPage 0번으로 이동
        //m_pPhaseStaminaBars[1]->GetWeponSelectButton()->SetEventLMB([this]() { this->ViewCardPage(1, 0); m_curSlot = 1; }); // 0번 슬롯 CardPage 0번으로 이동
        
    }
    
    // 페이지 버튼
    {
        m_pPageUpButton = m_uiFactory.CreateChild<Button>(m_name + L"_PageUp", Float2(74, 66), XMFLOAT3(-100, -350, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);
        assert(m_pPageUpButton);
        m_pPageUpButton->SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseout.png", L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseover.png");
        m_pPageUpButton->SetEventLMB([this]() { this->PageUp(m_curSlot); });

        m_pPageDownButton = m_uiFactory.CreateChild<Button>(m_name + L"_PageUp", Float2(74, 66), XMFLOAT3(-100, -150, 0), 0, XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this);
        assert(m_pPageDownButton);
        m_pPageDownButton->MirrorScaleY();
        m_pPageDownButton->SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseout.png", L"../Assets/UI/PLAY/PhaseScene/nextpage_mouseover.png");
        m_pPageDownButton->SetEventLMB([this]() { this->PageDown(m_curSlot); });
    }

    // 카드
    {
        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C0", Float2(205, 297), XMFLOAT3(-1300, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit0.png");
        m_pCards.back()->SetCardID(1);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C1", Float2(205, 297), XMFLOAT3(-1100, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit1.png");
        m_pCards.back()->SetCardID(2);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C2", Float2(205, 297), XMFLOAT3(-900, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit2.png");
        m_pCards.back()->SetCardID(3);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C3", Float2(205, 297), XMFLOAT3(-700, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit3.png");
        m_pCards.back()->SetCardID(4);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C4", Float2(205, 297), XMFLOAT3(-500, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit4.png");
        m_pCards.back()->SetCardID(5);

        m_pCards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C5", Float2(205, 297), XMFLOAT3(-300, -350, 0), XMFLOAT3(1, 1, 1), UIDirection::LeftTop, this));
        m_pCards.back()->ChangeTexture(L"../Assets/Test/unit5.png");
        m_pCards.back()->SetCardID(6);
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
    //const auto& card = myHands[slot].cards;
    //const int maxPage = myHands[slot].cards.size()/6;

    //for (int i = 0; i < m_pCards.size(); ++i) {
    //    auto id = card[i].dataID;
    //    m_pCards[i]->SetCardID(id);
    //    m_pCards[i]->ChangeTexture(m_cardManager.GetCardTexturePath(id));
    //}

    const auto& cards = m_player.hands[slot].cards;

    constexpr int CardSlotSize = 6; // 한 페이지 당 카드 갯수
    const int total = static_cast<int>(cards.size()); // 카드 총 사이즈

    const int viewCount = std::min(CardSlotSize, static_cast<int>(m_pCards.size()));

    if (total == 0)
    {
        m_curPage = 0;
        for (int i = 0; i < viewCount; ++i)
        {
            m_pCards[i]->SetCardID(0);
            m_pCards[i]->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        }
        return;
    }


    // total==0이면 maxPage=-1 (페이지가 "없다"는 의미)
    const int maxPage = GetMaxPage(slot);

    page = std::clamp(page, 0, maxPage);
    m_curPage = page;

    const int startIdx = page * CardSlotSize;

    
    for (int i = 0; i < viewCount; ++i)
    {
        const int idx = startIdx + i;

        if (idx < total)
        {
            const auto id = cards[idx].dataID;
            m_pCards[i]->SetCardID(id);
            m_pCards[i]->ChangeTexture(m_cardManager.GetCardTexturePath(id));
        }
        else
        {
            // 남는 슬롯은 카드 뒷면으로 채움
            m_pCards[i]->SetCardID(0); // 의미 없는 값이면 0 등으로 정리 (필요 없으면 제거)
            m_pCards[i]->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        }
    }
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

const int CardSelectionPanel::GetMaxPage(int slot)
{
    const auto& cards = m_player.hands[slot].cards;

    constexpr int CardSlotSize = 6; // 한 페이지 당 카드 갯수
    const int total = static_cast<int>(cards.size()); // 카드 총 사이즈
    // total==0이면 maxPage=-1 (페이지가 "없다"는 의미)
    return (total == 0) ? -1 : (total - 1) / CardSlotSize;
}
