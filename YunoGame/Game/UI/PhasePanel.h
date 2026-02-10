#pragma once
#include "Image.h"


class Card;
class CardSlot;
class CardManager;
class CardRangeManager;

struct UnitHand;
struct BattleResult;
struct ObstacleResult;

class PhasePanel : public Image
{
public:
    PhasePanel(UIFactory& uiFactory);
    virtual ~PhasePanel();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    void CreateChild() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::PhasePanel; }

    virtual void UpdatePanel(const BattleResult& battleResult) {}
    virtual void UpdatePanel(const ObstacleResult& obstacleResult) {}
    void SetupPanel(const int& pID, UnitHand* hand0, UnitHand* hand1, const CardManager& cardManger, const CardRangeManager& rangeManger)
    {
        m_pID = &pID;
        m_hands[0] = hand0;
        m_hands[1] = hand1;
        m_cardManager = &cardManger;
        m_rangeManager = &rangeManger;
        SetUpPanel();
    }

    //void SetUserID(int* pID) { m_pID = pID; }
    //const int GetUserID() { return *m_pID; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/white.png"); };    // 머테리얼 생성 (한 번만)
    
    virtual void SetUpPanel() = 0;

protected:
    

protected:


    const int* m_pID = nullptr; // 플레이어 ID (Team)
    std::array<const UnitHand*, 2> m_hands; // 본인 핸드
    const CardManager* m_cardManager = nullptr;
    const CardRangeManager* m_rangeManager = nullptr;
};

