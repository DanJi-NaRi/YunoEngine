#pragma once

#include "SceneState.h"
#include "C2S_BattlePackets.h"
#include "S2C_CardPackets.h"
#include "BattlePackets.h"

#include "CardManager.h"
#include "CardRangeManager.h"
#include "CardQueue.h"


class Minimap;
class CardConfirmPanel;
class CardSelectionPanel;

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
    int maxStamina = 0;
    int currentTile = 0;
};
// 얘가 뭘 가지고있어야 될까?
class GameManager
{
public:
    static void Initialize(GameManager* inst);
    void Init();
    static void Shutdown();
    static bool HasInstance();
    static GameManager& Get();
    
    void BindSceneManager(ISceneManager* sm) { m_sceneManager = sm; }
    yuno::game::YunoClientNetwork* GetClientNetwork() const {return m_clientNet;}
    void BindClientNetwork(yuno::game::YunoClientNetwork* net) { m_clientNet = net; }

    void SetSceneState(CurrentSceneState state);
    void SetState(CurrentSceneState state) { m_state = state; };    // SetSceneState의 함수는 호출안하고 m_state만 변경하고 싶을때 호출
    CurrentSceneState GetSceneState() { return m_state; };
    void RequestScenePop();
    void StartCountDown(int countTime,int S1U1,int S1U2,int S2U1,int S2U2);
    void Tick(float dt);

    void RequestAtk() {};

    void SendPacket(std::vector<std::uint8_t>&& bytes);

    void SetSlotIdx(int idx) { m_PID = idx; };
    int GetSlotiIdx() { return m_PID; };
    void SetMatchPlayerCount(int count) { m_matchPlayerCount = count; }
    int GetMatchPlayerCount() const { return m_matchPlayerCount; }
    bool HasOpponentInMatchRoom() const { return m_matchPlayerCount >= 2; }

    void SetMyPick(int index, PieceType type);
    void ResetMyPicks();
    bool HasTwoPicks() const;
    PieceType GetMyPiece(int idx) { return m_myPick[idx]; };

    bool ToggleReady();
    bool IsReady() const { return m_isReady; }
    void SetReadyStates(bool p1Ready, bool p2Ready);
    bool IsOpponentReady() const;

    void SubmitTurn(const std::vector<CardPlayCommand>& runtimeIDs);

    const int GetPID() { return m_PID; }

    void SendSurrender();//항복 패킷 보내기
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
    int GetCountDownSlotUnitId(int slotIndex, int unitIndex) const;

    bool IsBattleOngoing() const { return m_isBattleOngoing; }
    void SetBattleOngoing(bool v) { m_isBattleOngoing = v; }

    // 패널 사용 게터
    std::vector<Wdata>& GetWeapons() { return m_weapons; }


    // Picks
    PieceType& GetMyPick(size_t idx) { return m_myPick[idx]; } // idx: 0~1


    //승리플레이어 세터게터
    void SetWinnerPID(int pid) { m_winnerPID = pid; };
    int  GetWinnerPID() const { return m_winnerPID; };

    int GetCurrentRound() const { return m_currentRound; }
    void IncreaseRound() { ++m_currentRound; }
    void ResetRound() { m_currentRound = 0; }

    void RequestRevealStart() { m_shouldStartReveal = true; }
    bool GetRevealStart() const { return m_shouldStartReveal; }
    bool ConsumeRevealStart()
    {
        if (!m_shouldStartReveal) return false;
        m_shouldStartReveal = false;
        return true;
    }

    void SetMasterVolume(int v) { m_masterVolume = v; };
    int GetMasterVolume() const { return m_masterVolume; };
    void SetBGMVolume(int v) { m_bgmVolume = v; };
    int GetBGMVolume() const { return m_bgmVolume; };
    void SetSFXVolume(int v) { m_sfxVolume = v; };
    int GetSFXVolume() const { return m_sfxVolume; };

    void SetCoinToss(int c) { m_coinTossQueue.push(c); }
    int PopCoinToss()
    {
        if (m_coinTossQueue.empty())
            return 0;

        int v = m_coinTossQueue.front();
        m_coinTossQueue.pop();
        return v;
    }
private:
    static GameManager* s_instance;

private:
    ISceneManager* m_sceneManager = nullptr;

    CurrentSceneState m_state = CurrentSceneState::Title;

    PieceType m_myPick[2] = { PieceType::None, PieceType::None };
    PieceType m_lastPickedPiece = PieceType::None;

    int m_currentRound = 0;
    bool m_shouldStartReveal = false;

    int m_PID = 0; // 1 또는 2  >> 1이면 왼쪽 2면 오른쪽
    int m_matchPlayerCount = 0;

    //클라에서 전투중인지 확인
    bool m_isBattleOngoing = false;

    int m_winnerPID = 0;// 승리 플레이어

    bool m_isReady = false;
    bool m_p1Ready = false;
    bool m_p2Ready = false;

    int m_masterVolume = 5;
    int m_bgmVolume = 5;
    int m_sfxVolume = 5;

    std::queue<int> m_coinTossQueue;

    std::array<UnitHand,2> m_myHands;                                                          //UI 카드 선택용
    std::array<UnitHand,2> m_enemyHands;                                                       //UI 보여주기용
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
    std::array<Wdata, 2> m_myUIWeapons;
    std::array<Wdata, 2> m_enemyUIWeapons;
    bool m_uiWeaponDataReady = false;
    uint32_t m_uiWeaponDataVersion = 0;

public:
    bool IsEmptyWeaponData();
    void ResetWeaponData() { m_weapons.clear(); };
    void SetWeaponData(int _pId, int _slotId, int _weaponId, int _hp, int _stamina, int _currentTile);
    static int GetMaxStaminaByWeaponId(int weaponId);
    std::vector<Wdata> GetWeaponData() {
        return m_weapons;
    }

    void SetUIWeaponData(const std::array<Wdata, 4>& wdatas);
    bool IsUIWeaponDataReady() const { return m_uiWeaponDataReady; }
    uint64_t GetUIWeaponDataVersion() const { return m_uiWeaponDataVersion; }
    void ClearUIWeaponDataState();

    // 매 턴 매 슬롯 카드 하나 진행할 때마다 받아옴
private:

    std::queue<BattleResult> m_turnPkts;
    std::queue<BattleResult> m_revealBuffer;
    std::queue<ObstacleResult> m_obstaclePkts;

    Minimap* m_pMinimap = nullptr; // 미니맵
    CardConfirmPanel* m_pConfirmPanel = nullptr;
    CardSelectionPanel* m_pSelectionPanel = nullptr;

    public:
        // 패널 사용 게터

        // 카드 매니저
        CardManager& GetCardBasicManager() { return m_cardBasicMng; }
        CardRangeManager& GetCardRangeManager() { return m_cardRangeMng; }

        // 무기
        std::array<Wdata, 2>& GetMyUIWeapons() { return m_myUIWeapons; }
        std::array<Wdata, 2>& GetEnemyUIWeapons() { return m_enemyUIWeapons; }

        // 핸드
        std::array<UnitHand, 2>& GetMyHands() { return m_myHands; }
        std::array<UnitHand, 2>& GetEnemyHands() { return m_enemyHands; }

public:
    void ClearBattlePacket();
    void PushBattlePacket(const BattleResult _BattleResult);
    void PushRevealPacket(const BattleResult _BattleResult);
    BattleResult PopBattlePacket();
    BattleResult PopRevealPacket();
    bool IsEmptyBattlePacket();
    bool IsEmptyRevealQueue();
    void PushObstaclePacket(const ObstacleResult& obstacleResult);
    ObstacleResult PopObstaclePacket();
    bool IsEmptyObstaclePacket();

    void SetMinimap(Minimap* minimap);
    void SetConfirmPanel(CardConfirmPanel* pConfirmPanel);
    void SetSelectionPanel(CardSelectionPanel* pSelectionPanel);

    void SetUpPanels();
    void UpdatePanels(const BattleResult& obstacleResult);
    void UpdatePanels(const ObstacleResult& obstacleResult);
    void FlushPendingPanelUpdates();
    //void UpdateSelectionPanel(const UnitHand& playerInfo);

    private:
        std::queue<BattleResult> m_pendingBattlePanelUpdates;
};
