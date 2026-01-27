#pragma once

#include "SceneState.h"
#include "CardData.h"

class ISceneManager;
class ObjectManager;
namespace yuno::game
{
    class YunoClientNetwork;
}

// 얘가 뭘 가지고있어야 될까?
class GameManager
{
public:
    static void Initialize(GameManager* inst);
    static void Shutdown();
    static GameManager& Get();

    void BindSceneManager(ISceneManager* sm) { m_sceneManager = sm; }
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

    //void RoundInit(yuno::net::packets::S2C_Error data);

private:
    static GameManager* s_instance;

private:
    ISceneManager* m_sceneManager = nullptr;

    CurrentSceneState m_state = CurrentSceneState::Title;

    PieceType m_myPick[2] = { PieceType::None, PieceType::None };

    int m_PID = 0; // 1 또는 2  >> 1이면 왼쪽 2면 오른쪽

    bool m_isReady = false;

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
};
