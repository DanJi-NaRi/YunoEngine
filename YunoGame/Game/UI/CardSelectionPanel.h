#pragma once
#include "PhasePanel.h"

class Card;
class CardSlot;
class WeaponNameImage;
class PhaseStaminaBar;

struct UnitHand;
struct BattleResult;
struct ObstacleResult;

class CardSelectionPanel : public PhasePanel
{
public:
    CardSelectionPanel(UIFactory& uiFactory);
    virtual ~CardSelectionPanel();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    void CreateChild() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/tap_all_click.png"); };    // 머테리얼 생성 (한 번만)

    virtual void SetUpPanel() override;

    void UpdatePanel(const BattleResult& battleResult) override;
    void UpdatePanel(const ObstacleResult& obstacleResult) override;

    void SetID(int* pID) { m_pID = pID; }
    const int GetID() { return *m_pID; }

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardSelectionPanel; }

protected:
    // 덱 정보
    int* m_pID = nullptr; // 플레이어 ID (Team)

    // 세팅된 카드 / 스냅 위치 정보
    std::vector<Card*> m_pCards;                   // 꺼내갈 카드
    std::vector<CardSlot*> m_pCardSlots;           // 카드 세팅할 슬롯 위젯
    int m_cardPage;                                 // 표기할 카드 페이지
    
    std::array<PhaseStaminaBar*, 2> m_pPhaseStaminaBars;
    WeaponNameImage* m_pWeaponIMG = nullptr;
    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
};

