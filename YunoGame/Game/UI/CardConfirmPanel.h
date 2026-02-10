#pragma once
#include "PhasePanel.h"

class Card;
class CardSlot;
class CardConfirmButton;
class CardCancelButton;
class CardConfirmArea;

class CardConfirmPanel : public PhasePanel
{
public:
    CardConfirmPanel(UIFactory& uiFactory);
    virtual ~CardConfirmPanel();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();
    //void LoadDeck(); void DataSend(); // 외부 서비스로 분리
    //void CardDrag();    // 카드 드래그

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardConfirmPanel; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/window_popup_battlesetup.png"); };    // 머테리얼 생성 (한 번만)

    virtual void SetUpPanel() override;

    void UpdatePanel(const BattleResult& battleResult);
    void UpdatePanel(const ObstacleResult& obstacleResult);

    void UpdateCardSlot();
    void ClearSlot();

protected:

    int* m_pID = nullptr; // 플레이어 ID (Team)
    

    int m_openSlot; // 드래그 앤 드랍이 가능한 현재 카드 슬롯 번호
    // 세팅된 카드 / 스냅 위치 정보
    //std::queue<Card*> m_setCards;                   // 세팅한 카드 // 슬롯 4개로 할 것 // m_setCardSlots[i]->GetCard;와 동일
    //std::vector<CardSlot*> m_setCardSlots;          // 카드 세팅할 슬롯 위젯
    std::vector<CardConfirmArea*> m_setCardSlots;          // 카드 세팅할 슬롯 위젯
    CardConfirmButton* m_cardConfirmButton;         // 카드 컨펌(선택 완료) 버튼
    CardCancelButton* m_cardCancelButton;           // 카드 캔슬(선택 초기화) 버튼
    
    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
};

