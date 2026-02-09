#pragma once

#include "SceneState.h"
#include "CardData.h"
#include "C2S_BattlePackets.h"
#include "S2C_CardPackets.h"
#include "BattlePackets.h"

#include "CardManager.h"
#include "CardRangeManager.h"
#include "CardQueue.h"

class ISceneManager;
class ObjectManager;
namespace yuno::game
{
    class YunoClientNetwork;
}
struct PendingEmote //이모지
{
    uint8_t pid;
    uint8_t emoteId;
};
struct ClientCardInfo //UI에 적용하기 위한 데이터 저장
{
    //uint8_t slotID;         // 0 1 2 3 
    //uint8_t weaponID;       // 어떤 무기인지 (1~6) //UnitSlot
    uint32_t runtimeID;
    uint32_t dataID;
};
struct UnitHand
{
    std::vector<ClientCardInfo> cards;
};
struct Wdata
{
    int pId = 0;
    int slotId = 0;
    int weaponId = 0;
    int hp = 0;
    int stamina = 0;
    int currentTile = 0;
};
// 얘가 뭘 가지고있어야 될까?
class GameManager
{
public:
    static void Initialize(GameManager* inst);
    void Init();
    static void Shutdown();
    static GameManager& Get();
    
    void BindSceneManager(ISceneManager* sm) { m_sceneManager = sm; }
    yuno::game::YunoClientNetwork* GetClientNetwork() const {return m_clientNet;}
    void BindClientNetwork(yuno::game::YunoClientNetwork* net) { m_clientNet = net; }

    void SetSceneState(CurrentSceneState state);
    CurrentSceneState GetSceneState() { return m_state; };
    void StartCountDown(int countTime,int S1U1,int S1U2,int S2U1,int S2U2);
    void Tick(float dt);

    void RequestAtk() {};

    void SendPacket(std::vector<std::uint8_t>&& bytes);

    void SetSlotIdx(int idx) { m_PID = idx; };
    int GetSlotiIdx() { return m_PID; };

    void SetMyPick(int index, PieceType type);
    void ResetMyPicks();
    bool HasTwoPicks() const;
    PieceType GetMyPiece(int idx) { return m_myPick[idx]; };

    bool ToggleReady();
    bool IsReady() const { return m_isReady; }

    void SubmitTurn(const std::vector<CardPlayCommand>& runtimeIDs);

    //void RoundInit(yuno::net::packets::S2C_Error data);

// 카드 관련 변수와 함수
private:
    CardManager m_cardBasicMng;
    CardRangeManager m_cardRangeMng;

public:
    //초기 카드 생성
    void InitHands(const std::vector<yuno::net::packets::CardSpawnInfo>& cards);
    //카드 생성 저장
    void AddCards(const std::vector<yuno::net::packets::CardSpawnInfo>& cards);
    //카드 제거
    void RemoveCard(uint32_t runtimeID);
    //카드 선택용 인덱스 -> runtimeID
    uint32_t GetMyCardRuntimeID(int unitSlot, int index) const;
    //UI 표시용 runtimeID -> dataID
    uint32_t GetCardDataID(uint32_t runtimeID) const;

    //~ 추가 후보 카드 저장 및 가져와서 서버에 보내고 배열 비우기
    void SetDrawCandidates(const std::vector<yuno::net::packets::CardSpawnInfo>& cards);
    void SendSelectCard(int index);
    const std::vector<ClientCardInfo>& GetDrawCandidates() const;
    void ClearDrawCandidates();
    //~ 여기까지

    //~카드큐 관련
    bool PushCardCommand(const CardPlayCommand& cmd);
    void ClearCardQueue();

    bool IsCardQueueEmpty() const;
    bool IsCardQueueFull() const;
    
    const std::vector<CardPlayCommand>& GetCardQueue() const;
    //~여기까지
    
    //~여기부터 이모지
    void PushEmote(uint8_t pid, uint8_t emoteId);
    bool PopEmote(PendingEmote& out);
    //~여기까지
    void SendEmote(uint8_t emoteId);
    const CardData GetCardData(uint32_t runtimeID);
    const CardEffectData* GetCardEffectData(uint32_t runtimeID);
    const RangeData* GetRangeData(uint32_t runtimeID);

    bool IsCountdownActive() const;
    bool IsCountdownFinished() const;
    int    GetCountdownNumber() const;
private:
    static GameManager* s_instance;

private:
    ISceneManager* m_sceneManager = nullptr;

    CurrentSceneState m_state = CurrentSceneState::Title;

    PieceType m_myPick[2] = { PieceType::None, PieceType::None };
    PieceType m_lastPickedPiece = PieceType::None;


    int m_PID = 0; // 1 또는 2  >> 1이면 왼쪽 2면 오른쪽

    bool m_isReady = false;

    UnitHand m_myHands[2];                                                             //UI 카드 선택용
    UnitHand m_enemyHands[2];                                                       //UI 보여주기용
    std::unordered_map<uint32_t, uint32_t> m_CardRuntimeIDs;    //엑셀로드용
    std::vector<ClientCardInfo> m_drawCandidates;                         //추가 후보 카드 임시 저장소
    CardQueue m_cardQueue;

    std::queue<PendingEmote> m_pendingEmotes;

    bool m_countdownActive = false;
    bool m_countdownFinished = false;
    float m_countdownRemaining = 0.0f;

    int m_S1U1 = 0;
    int m_S1U2 = 0;
    int m_S2U1 = 0;
    int m_S2U2 = 0;

    yuno::game::YunoClientNetwork* m_clientNet = nullptr;


    // 라운드 초기화 데이터
private:
    // m_weapons에 데이터 담는 함수 만들기
    // m_weapons에 데이터 가져오는 함수 만들기
    std::vector<Wdata> m_weapons;

public:
    bool IsEmptyWeaponData();
    void ResetWeaponData() { m_weapons.clear(); };
    void SetWeaponData(int _pId, int _slotId, int _weaponId, int _hp, int _stamina, int _currentTile);
    std::vector<Wdata> GetWeaponData() {
        return m_weapons;
    }


    // 매 턴 매 슬롯 카드 하나 진행할 때마다 받아옴
private:

    std::queue<BattleResult> m_turnPkts;
    std::queue<ObstacleResult> m_obstaclePkts;

public:
    void PushBattlePacket(const BattleResult _BattleResult);
    BattleResult PopBattlePacket();
    bool IsEmptyBattlePacket();

    void PushObstaclePacket(const ObstacleResult& obstacleResult);
    ObstacleResult PopObstaclePacket();
    bool IsEmptyObstaclePacket();
};
