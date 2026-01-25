#pragma once
#include "Widget.h"

class Card;



class CardTable : public Widget
{
public:
    CardTable();
    virtual ~CardTable() = default;

    //void LoadDeck(); void DataSend(); // 외부 서비스로 분리
    void CardSet();     // 카드 세팅
    //void CardDrag();    // 카드 드래그

    virtual WidgetType GetWidgetType() override { return WidgetType::Widget; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardTable; }

protected:
    std::vector<std::unique_ptr<Card>> m_deck;      // 카드 덱 // 그리드 시스템 이용할수도??
    std::queue<Card*> m_SetCards;                   // 세팅한 카드 // 슬롯 4개
    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
};

