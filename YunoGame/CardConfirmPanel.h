#pragma once
#include "Image.h"

class Card;
class CardSlot;
class CardConfirmButton;
class CardCancelButton;


class CardConfirmPanel : public Image
{
public:
    CardConfirmPanel(UIFactory& uiFactory);
    virtual ~CardConfirmPanel();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
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
    // 세팅된 카드 / 스냅 위치 정보
    std::queue<Card*> m_SetCards;                   // 세팅한 카드 // 슬롯 4개로 할 것
    std::vector<CardSlot*> m_SetCardSlots;          // 카드 세팅할 슬롯 위젯
    CardConfirmButton* m_CardConfirmButton;         // 카드 컨펌(선택 완료) 버튼
    CardCancelButton* m_CardCancelButton;           // 카드 캔슬(선택 초기화) 버튼
    
    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
};

