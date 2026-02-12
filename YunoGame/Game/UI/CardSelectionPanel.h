#pragma once
#include <array>
#include "PhasePanel.h"

class Button;
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

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardSelectionPanel; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/tap_all_click.png"); };    // 머테리얼 생성 (한 번만)

    void UpdatePanel(const BattleResult& battleResult) override;
    void UpdatePanel(const ObstacleResult& obstacleResult) override;

    void SetID(int* pID) { m_pID = pID; }
    const int GetID() { return *m_pID; }

    std::vector<ClientCardInfo> GetCardsByFilter(int slot, const CardType filter);

    // 페이지
    void ViewCardPage(int slot, int page); // 몇 번째 무기 페이지인지 
    
    void PageUp(int slot);
    void PageDown(int slot);

    const int GetMaxPage(int slot);

    WeaponNameImage* GetWeaponNameImage() const { return m_pWeaponIMG; }

protected:
    // 덱 정보
    int* m_pID = nullptr; // 플레이어 ID (Team)

    // 필터 버튼
    std::array<Button*, 5> m_pFilterButtons;

    // 세팅된 카드 / 스냅 위치 정보
    std::vector<Card*> m_pCards;                   // 꺼내갈 카드
    std::vector<CardSlot*> m_pCardSlots;           // 카드 세팅할 슬롯 위젯
    int m_curPage;                                 // 표기할 카드 페이지
    CardType m_curFilter;                          // 표기할 카드 종류
    int m_curSlot;                                 // 현재 보고있는 슬롯
    Button* m_pPageUpButton = nullptr;
    Button* m_pPageDownButton = nullptr;
    std::array<PhaseStaminaBar*, 2> m_pPhaseStaminaBars;
    WeaponNameImage* m_pWeaponIMG = nullptr;
    //std::unique_ptr<Minimap> m_miniMap;           // 미니맵 // 스폰 포지션 따로 받기?
    Card* m_pSelectedCard = nullptr;

public:
    Card* GetSelectedCard() const { return m_pSelectedCard; }
    void ClearSelectedCard() { m_pSelectedCard = nullptr; }
    int GetCurrentSlot() const { return m_curSlot; }
    int GetCurrentPage() const { return m_curPage; }
    void RefreshCardVisualState();

};

