#pragma once
#include "Image.h"
#include "GameManager.h"

class Card;
class CardSlot;
class CardManager;
class CardRangeManager;

struct PlayerData {
    PlayerData() = delete;
    PlayerData(std::array<Wdata, 2>& weapons, std::array<UnitHand, 2>& hands)
        : weapons(weapons), hands(hands) {
    }
    std::array<Wdata, 2>& weapons;
    std::array<UnitHand, 2>& hands;
};

enum class TeamData : uint8_t {
    Default,
    Red,  // Team1
    Blue, // Team2
};

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

    //void SetUserID(int* pID) { m_pID = pID; }
    //const int GetUserID() { return *m_pID; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/white.png"); };    // 머테리얼 생성 (한 번만)

    const TeamData GetTeamData() { return static_cast<TeamData>(m_pID); }
    const int GetPID() { return m_pID; }

protected:

    // 매니저
    GameManager& m_gameManager;
    CardManager& m_cardManager;
    CardRangeManager& m_rangeManager;
    // 플레이어 데이터
    PlayerData m_player;
    PlayerData m_enemy;
    int m_pID; // 플레이어 ID (Team)
};

