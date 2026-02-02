#pragma once

#include "SceneState.h"
#include "CardData.h"
#include "C2S_BattlePackets.h"
#include "S2C_StartCardList.h"
#include "BattlePackets.h"

class ISceneManager;
class ObjectManager;
namespace yuno::game
{
    class YunoClientNetwork;
}

struct ClientCardInfo //UI에 적용하기 위한 데이터 저장
{
    uint32_t runtimeID;
    uint32_t dataID;
};
// 얘가 뭘 가지고있어야 될까?
class GameManager
{
public:
    static void Initialize(GameManager* inst);
    static void Shutdown();
    static GameManager& Get();

    void BindSceneManager(ISceneManager* sm) { m_sceneManager = sm; }
    yuno::game::YunoClientNetwork* GetClientNetwork() const {return m_clientNet;}
    void BindClientNetwork(yuno::game::YunoClientNetwork* net) { m_clientNet = net; }

    void SetSceneState(CurrentSceneState state);
    void StartCountDown(int countTime,int S1U1,int S1U2,int S2U1,int S2U2);
    void Tick(float dt);

    void RequestAtk() {};

    void SendPacket(std::vector<std::uint8_t>&& bytes);

    void SetSlotIdx(int idx) { m_PID = idx; };
    int GetSlotiIdx() { return m_PID; };

    void SetMyPick(int index, PieceType type);
    bool HasTwoPicks() const;
    PieceType GetMyPiece(int idx) { return m_myPick[idx]; };

    bool ToggleReady();
    bool IsReady() const { return m_isReady; }

    void SubmitTurn(const std::vector<CardPlayCommand>& runtimeIDs);

    //void RoundInit(yuno::net::packets::S2C_Error data);

    //카드 생성 저장
    void SetCards(const std::vector<yuno::net::packets::CardSpawnInfo>& cards);
    //카드 선택용 인덱스 -> runtimeID
    uint32_t GetCardRuntimeIDByIndex(int index) const
    {
        if (index < 0 || index >= (int)m_Cards.size())
            return 0;

        return m_Cards[index].runtimeID;
    }
    //UI 표시용 runtimeID -> dataID
    uint32_t GetCardDataID(uint32_t runtimeID) const
    {
        auto it = m_CardRuntimeIDs.find(runtimeID);
        if (it == m_CardRuntimeIDs.end())
            return 0;

        return it->second;
    }
private:
    static GameManager* s_instance;

private:
    ISceneManager* m_sceneManager = nullptr;

    CurrentSceneState m_state = CurrentSceneState::Title;

    PieceType m_myPick[2] = { PieceType::None, PieceType::None };

    int m_PID = 0; // 1 또는 2  >> 1이면 왼쪽 2면 오른쪽

    bool m_isReady = false;

    std::vector<ClientCardInfo> m_Cards;                                        //UI용 순서 있음
    std::unordered_map<uint32_t, uint32_t> m_CardRuntimeIDs;   //엑셀로드용

    bool m_countdownActive = false;
    float m_countdownRemaining = 0.0f;

    int m_S1U1 = 0;
    int m_S1U2 = 0;
    int m_S2U1 = 0;
    int m_S2U2 = 0;

    yuno::game::YunoClientNetwork* m_clientNet = nullptr;


    // 라운드 초기화 데이터
private:
    struct Wdata 
    {
        int pId = 0;
        int slotId = 0;
        int weaponId = 0;
        int hp = 0;
        int stamina = 0;
        int currentTile = 0;
    };
    // m_weapons에 데이터 담는 함수 만들기
    // m_weapons에 데이터 가져오는 함수 만들기
    std::vector<Wdata> m_weapons;

public:
    void SetWeaponData(int _pId, int _slotId, int _weaponId, int _hp, int _stamina, int _currentTile);
    std::vector<Wdata> GetWeaponData() {
        return m_weapons;
    }

    // 매 턴 매 슬롯 카드 하나 진행할 때마다 받아옴
private:

    std::queue<BattleResult> m_turnPkts;

public:
    void PushBattlePacket(const BattleResult& _BattleResult);
    BattleResult PopBattlePacket();
    bool IsEmptyBattlePacket();
};
