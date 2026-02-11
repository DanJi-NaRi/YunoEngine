#pragma once
#include "PhasePanel.h"

class Card;
class CardSlot;
class CardConfirmButton;
class CardCancelButton;
class CardConfirmArea;

class Minimap;
class CardSelectionPanel;


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


    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardConfirmPanel; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/window_popup_battlesetup.png"); };

    void UpdatePanel(const BattleResult& battleResult);
    void UpdatePanel(const ObstacleResult& obstacleResult);

    void UpdateCardSlot();
    void ClearSlot();
    void BindSelectionPanel(CardSelectionPanel* panel) { m_pSelectionPanel = panel; }

    // 나중에 View 만들어서 대체하기
    void SetMinimap(Minimap* pMinimap) { m_pMinimap = pMinimap; }

protected:
    void SubmitCurrentSelection();
    bool IsCardAlreadyQueued(uint32_t runtimeID) const;
    bool HasEnoughStaminaForCard(int unitSlot, uint32_t runtimeID) const;
    void SyncSimulatedStaminaFromPlayer();

    


protected:

    //int m_confirmReady; // 모든 슬롯에 바인딩했는지


    int m_openSlot;
    bool m_dirChoice = false;
    bool m_hasSimulatedStamina = false;
    std::array<int, 2> m_originalStamina = { 0, 0 };
    std::array<int, 2> m_simulatedStamina = { 0, 0 };
    std::vector<CardConfirmArea*> m_setCardSlots;
    CardConfirmButton* m_cardConfirmButton = nullptr;
    CardCancelButton* m_cardCancelButton = nullptr;
    CardSelectionPanel* m_pSelectionPanel = nullptr;

    
    Minimap* m_pMinimap = nullptr;                  // 미니맵 
};

