#pragma once
#include "Image.h"

class Card;
class CardSlot;


class CardTable : public Image
{
public:
    CardTable(UIFactory& uiFactory);
    virtual ~CardTable();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    void CreateChild() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();
    //void LoadDeck(); void DataSend(); // 외부 서비스로 분리
    //void CardDrag();    // 카드 드래그

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardTable; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/white.png"); };    // 머테리얼 생성 (한 번만)
protected:
    // 덱 정보
    std::vector<std::unique_ptr<Card>> m_deck;      // 카드 덱 // 그리드 시스템 이용할수도??
    
    // 세팅된 카드 / 스냅 위치 정보
    std::queue<Card*> m_SetCards;                   // 세팅한 카드 // 슬롯 4개로 할 것
    std::vector<CardSlot*> m_SetCardSlots;          // 카드 세팅할 슬롯 위젯


    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
};

