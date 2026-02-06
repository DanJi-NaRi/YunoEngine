#pragma once
#include <vector>
#include <cstdint>

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct UnitStateDelta
    {
        uint8_t ownerSlot;       // 플레이어 식별 (1,2)
        uint8_t unitLocalIndex;  // 해당 플레이어의 유닛 (1,2)

        uint8_t hp;
        uint8_t stamina;
        uint8_t targetTileID;
        uint8_t isEvent;         // 0이면 충돌x, 1이면 충돌o

        void Serialize(ByteWriter& w) const;
        static UnitStateDelta Deserialize(ByteReader& r);
    };

    struct ObstacleState
    {
        uint8_t obstacleID;                 // 장애물 타입 ID (0~3) 0은 장애물 없음
        std::vector<uint8_t> tileIDs;     // 장애물이 점유하는 타일들ID (1~35)
        std::array<UnitStateDelta, 4> unitState;

        void Serialize(ByteWriter& w) const;
        static ObstacleState Deserialize(ByteReader& r);
    };
    // 이게 서버가 시뮬 돌려서 결과 보내주는 패킷
    // 카드 8개의 발동 순서는 서버가 정렬해서 시뮬돌리고 패킷 8번 보낸다.
    struct S2C_BattleResult
    {
        uint32_t runtimeCardId;  // 선택한 카드 ID >> 카드에 접근해서 >> CaraData 공격력, 범위, 방향 (이거만으로 시뮬을 돌릴수 있는데)
        uint8_t dir;             // 카드배치때 선택한 방향 
        uint8_t ownerSlot;       // PID (1,2)
        uint8_t unitLocalIndex;  // 해당 플레이어의 유닛 (1,2)
        uint32_t actionTime;      // 해당 결과 액션 타임

        std::vector<std::array<UnitStateDelta, 4>> order;   // 카드 시뮬 결과
        void Serialize(ByteWriter& w) const;
        static S2C_BattleResult Deserialize(ByteReader& r);
    };

    struct S2C_ObstacleResult
    {
        std::vector<ObstacleState> obstacles;

        void Serialize(ByteWriter& w) const;
        static S2C_ObstacleResult Deserialize(ByteReader& r);
    };
}










// S >> C 패킷 8개는 한번에 보내 
// C GameManager Queue 여기에 8개가 들어가
// C Queue를 Pop하면서 실행할 결과를 꺼내 (카드ID, 시간)
// 1. 8개를 한번에보내 30초 (얘도 넉넉하게 보내) >> 처음 패킷 받을 때 생기는 지연 >> 서버가 다음 카드제출해 스냅샷 (두 컴퓨터 지연)
// 2. 1개씩보내 >> 각 클라에서 패킷을 받은 순간부터 Time (A가 좀 늦게 받았어 (시간을 좀 넉넉하게 보내)) >>  >> 마지막 패킷이 3초 >>
// 3. 민경이 처럼하면 행동 끝내고 >> 나 다했어 >> 서버 (둘 다 다했어) >> 다음 카드 제출해! (스냅샷)    // 디버깅하려면 합을 맞춰야 돼
