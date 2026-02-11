#include "pch.h"
#include "CardConfirmArea.h"

#include "IInput.h"
#include "Card.h"

CardConfirmArea::CardConfirmArea(UIFactory& uiFactory) : Slot(uiFactory)
{
    Clear();
}

CardConfirmArea::~CardConfirmArea()
{
    Clear();
}

bool CardConfirmArea::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Slot::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_snapPoint.useSnap = false; // 스냅 사용하지 않는 이벤트 쿼드 용도
    m_direction = Direction::None;

    SetSnapPoint(5.0f, WidgetClass::Card);
    return true;
}

bool CardConfirmArea::Start()
{
    Slot::Start();

    return true;
}

bool CardConfirmArea::Update(float dTime) {
    Slot::Update(dTime);

    /*if (m_pCard)this->ChangeTexture(m_pCard->GetTexturePath()); 
    else this->ChangeTexture(m_texturePathBk);*/

    return true;
}

bool CardConfirmArea::Submit(float dTime)
{
    Slot::Submit(dTime);
    return true;
}


void CardConfirmArea::Clear()
{
    m_pCard = nullptr;
    m_direction = Direction::None;
    m_runtimeCardID = 0;
    m_cardSlotID = -1;
}

void CardConfirmArea::CleanSetup()
{
    DetachSnap();
    Clear();
    this->ChangeTexture(m_texturePathBk);
}

bool CardConfirmArea::Event(float dTime)
{
    //std::cout << "EVENT!!!" << std::endl;
    assert(m_snapPoint.pSnapOwner);

    Card* snappedCard = dynamic_cast<Card*>(m_snapPoint.pSnapOwner);
    SetCard(snappedCard);

    m_pCard = dynamic_cast<Card*>(m_snapPoint.pSnapOwner);

    if (m_pCard)
        this->ChangeTexture(m_pCard->GetTexturePath());
        
    // 이벤트 공간
    
    return true;
}


