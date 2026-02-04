#include <iostream>
#include <array>
#include <algorithm>
#include <cstdlib>

#include "TurnManager.h"
#include "MatchManager.h"
#include "YunoServerNetwork.h"
#include "RoundController.h"

#include "PacketBuilder.h"
#include "S2C_BattlePackets.h"
#include "S2C_StartCardList.h"
#include "ServerCardManager.h"

#include "BattleState.h"

static bool IsALess(const ResolvedCard& a, const ResolvedCard& b, bool& coinTossUsed)
{
    if (a.speed != b.speed) return a.speed < b.speed;
    if (a.weaponId != b.weaponId) return a.weaponId < b.weaponId;

    // 완전 동일 => 코인토스
    coinTossUsed = true;
    return (std::rand() & 1) == 0; // true면 a가 먼저
}

namespace yuno::server
{
    namespace
    {
        constexpr int kGridRows = 5;
        constexpr int kGridColumns = 7;
        constexpr int kGridSize = kGridRows * kGridColumns;

        struct TilePos
        {
            int x = 0;
            int y = 0;
        };

        bool IsInBounds(const TilePos& pos)
        {
            return pos.x >= 0 && pos.x < kGridColumns
                && pos.y >= 0 && pos.y < kGridRows;
        }

        TilePos TileIdToPos(uint8_t tileId)
        {
            int id = static_cast<int>(tileId) - 1;
            return { id % kGridColumns, id / kGridColumns };
        }

        uint8_t PosToTileId(const TilePos& pos)
        {
            return static_cast<uint8_t>((pos.y * kGridColumns + pos.x) + 1);
        }

        TilePos DirectionToDelta(int moveX, int moveY, Direction dir)
        {
            int dx = std::abs(moveX);
            int dy = std::abs(moveY);
            switch (dir)
            {
            case Direction::Up: return { 0, -dx };
            case Direction::Down: return { 0, dx };
            case Direction::Left: return { -dx, 0 };
            case Direction::Right: return { dx, 0 };
            case Direction::UpLeft: return { -dy, -dy };
            case Direction::UpRight: return { dy, -dy };
            case Direction::DownLeft: return { -dy, dy };
            case Direction::DownRight: return { dy, dy };
            case Direction::Same:
            case Direction::None:
            default:
                return { 0, 0 };
            }
        }
    }

    TurnManager::TurnManager(
        MatchManager& match,
        YunoServerNetwork& network,
        ServerCardRuntime& runtime,
        ServerCardManager& cardDB,
        RoundController& roundController)
        : m_match(match)
        , m_network(network)
        , m_runtime(runtime)
        , m_cardDB(cardDB)
        , m_roundController(roundController)
    {
    }

    void TurnManager::SubmitTurn(
        uint64_t sessionId,
        const std::vector<CardPlayCommand>& commands)
    {
        int idx = m_match.FindSlotBySessionId(sessionId);
        if (idx < 0) return;

        m_turnCards[idx] = commands;
        m_submitted[idx] = true;

        std::cout << "[Server] Player " << idx
            << " submitted " << commands.size()
            << " commands\n";

        // 두 플레이어가 모두 CardSubmit하면 아래 함수 실행됨
        TryResolveTurn();
    }

    void TurnManager::BroadcastTestBattleResult(
        const ResolvedCard& c)
    {
        using namespace yuno::net::packets;

        S2C_BattleResult pkt;

        // 쓰레기 값 들어가고 있어서 테스트를 위해 하드코딩
        //pkt.runtimeCardId = c.runtimeId;
        //pkt.ownerSlot = static_cast<uint8_t>(c.ownerSlot);
        pkt.runtimeCardId = c.runtimeId;
        pkt.ownerSlot = 1;
        pkt.unitLocalIndex = 1;
        pkt.dir = 0;

        //pkt.ownerSlot = static_cast<uint8_t>(c.ownerSlot);
        //pkt.unitLocalIndex = static_cast<uint8_t>(c.localIndex);
        //pkt.dir = static_cast<uint8_t>(c.dir);

        //  테스트용 하드코딩 결과
        std::array<UnitStateDelta, 4> us;
        us[0] = { 1, 1, 80, 100, 9, 1 };
        us[1] = { 1, 2, 90, 100, 12, 0 };
        us[2] = { 2, 1, 70, 100, 23, 0 };
        us[3] = { 2, 2, 90, 100, 27, 0 };
        pkt.order.push_back(us);


        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_BattleResult,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        //  두 클라에게 동일한 결과
        m_network.Broadcast(std::move(bytes));

        std::cout
            << "[Server] Broadcast BattleResult runtime="
            << c.runtimeId << "\n";
    }


    void TurnManager::TryResolveTurn()
    {
        if (!(m_submitted[0] && m_submitted[1]))
            return;

        std::cout << "[Server] Both players submitted turn\n";

        std::vector<ResolvedCard> slotCards[2]; // 0 은 1P, 1은 2P


        for (int slot = 0; slot < 2; ++slot)
        {
            int localIndex = 0;

            for (const CardPlayCommand& cmd : m_turnCards[slot]) // 플레이어가 제출한 카드들
            {
                uint32_t runtimeId = cmd.runtimeID;
                uint32_t dataId = m_runtime.GetDataID(runtimeId); // 런타임ID >> 데이터ID로 변경
                const CardData& card = m_cardDB.GetCardData(dataId); // 데이터ID로 카드DB에서 카드 데이터 가져옴

                slotCards[slot].push_back({ // 슬롯에 장착한 카드 정보 삽입
                    runtimeId,
                    dataId,
                    card.m_allowedUnits,
                    card.m_speed,
                    slot,
                    localIndex++,
                    cmd.dir
                    });

                std::cout
                    << "[Server] Received Card | slot=" << slot
                    << " local=" << localIndex - 1
                    << " runtimeId=" << runtimeId
                    << " dataId=" << dataId
                    << " name=" << card.m_name
                    << " speed=" << card.m_speed
                    << " weapon=" << card.m_allowedUnits
                    << " Direction=" << static_cast<int>(cmd.dir)
                    << "\n";
            }
        }

        auto& p1 = g_battleState.players[0];
        auto& p2 = g_battleState.players[1];

        std::array<UnitState*, 4> units = {
            &p1.unit1,
            &p1.unit2,
            &p2.unit1,
            &p2.unit2
        };

        std::vector<int> grid(kGridSize + 1, -1);   // size 36 , 0은 Error, 1~35가 TileId
        for (int i = 0; i < static_cast<int>(units.size()); ++i)
        {
            uint8_t tileId = units[i]->tileID;
            if (tileId != 0 && tileId <= kGridSize)
            {
                grid[tileId] = i;       // 서버 내부 그리드에 유닛 배치
                std::cout << static_cast<int>(tileId) << "  ";
            }
        }

        // 변경된 정보를 기반으로 스냅샷 찍어서 패킷 만들기, (UnitStateDelta) 만드는 애임
        auto buildDeltaSnapshot = [&](int eventUnitIndex, bool hasEvent)
            {
                std::array<yuno::net::packets::UnitStateDelta, 4> deltas{};
                int deltaIndex = 0;

                for (int p = 0; p < 2; ++p)
                {
                    const auto& player = g_battleState.players[p];

                    const UnitState* playerUnits[2] = {
                        &player.unit1,
                        &player.unit2
                    };

                    for (int u = 0; u < 2; ++u)
                    {
                        const UnitState* unit = playerUnits[u];
                        bool isEventUnit = (deltaIndex == eventUnitIndex);

                        deltas[deltaIndex++] = {
                            player.PID,     
                            unit->slotID,    
                            unit->hp,
                            unit->stamina,
                            unit->tileID,
                            static_cast<uint8_t>(hasEvent && isEventUnit)
                        };
                    }
                }
                return deltas;
            };

        auto getUnitIndexForCard = [&](int ownerSlot, const CardData& card) -> int
            {
                auto& player = (ownerSlot == 0) ? g_battleState.players[0] : g_battleState.players[1];

                if (player.unit1.WeaponID == card.m_allowedUnits)
                    return (ownerSlot == 0) ? 0 : 2;
                if (player.unit2.WeaponID == card.m_allowedUnits)
                    return (ownerSlot == 0) ? 1 : 3;
                return (ownerSlot == 0) ? 0 : 2;
            };

        auto applyMove = [&](int unitIndex, const CardMoveData& moveData, Direction dir) -> bool
            {
                std::cout << "Enter ApplyMove , ";
                UnitState& unit = *units[unitIndex];
                if (unit.tileID == 0)
                    return false;

                TilePos current = TileIdToPos(unit.tileID);
                TilePos delta = DirectionToDelta(moveData.m_moveX, moveData.m_moveY, dir);
                TilePos target = { current.x + delta.x, current.y + delta.y };

                if (!IsInBounds(target))
                {
                    std::cout << "IsInBounds false" << std::endl;
                    return true;
                }


                int steps = std::max(std::abs(delta.x), std::abs(delta.y));
                int stepX = (delta.x == 0) ? 0 : (delta.x > 0 ? 1 : -1);
                int stepY = (delta.y == 0) ? 0 : (delta.y > 0 ? 1 : -1);

                TilePos probe = current;
                for (int step = 0; step < steps; ++step)
                {
                    probe.x += stepX;
                    probe.y += stepY;
                    if (!IsInBounds(probe))
                    {
                        std::cout << "IsInBounds false" << std::endl;
                        return true;
                    }

                    uint8_t probeId = PosToTileId(probe);
                    if (grid[probeId] != -1)
                    {
                        std::cout << "IsInGrif flase" << std::endl;
                        return true;
                    }
                }

                uint8_t oldTile = unit.tileID;
                uint8_t newTile = PosToTileId(target);
                grid[oldTile] = -1;
                grid[newTile] = unitIndex;
                unit.tileID = newTile;
                std::cout << "oldPos : " << static_cast<int>(oldTile) << "newPos : " << static_cast<int>(newTile) << std::endl;
                return false;
            };

        auto applyCard = [&](const ResolvedCard& card)
            {
                const CardData& cardData = m_cardDB.GetCardData(card.dataId);
                int unitIndex = getUnitIndexForCard(card.ownerSlot, cardData);
                bool eventOccurred = false;

                if (cardData.m_cost > 0)
                {
                    UnitState& unit = *units[unitIndex];
                    unit.stamina = static_cast<uint8_t>(
                        std::max(0, static_cast<int>(unit.stamina) - cardData.m_cost));
                    std::cout << "stamina : " << static_cast<int>(unit.stamina) << std::endl;
                }
                else 
                {
                    std::cout << "do not Use Cost" << std::endl;
                }

                if (cardData.m_type == CardType::Move)
                {
                    if (const auto* moveData = m_cardDB.GetMoveData(card.dataId))
                    {
                        eventOccurred = applyMove(unitIndex, *moveData, card.dir);
                    }
                }
                else 
                {
                    std::cout << "not Move Card" << std::endl;
                }

                using namespace yuno::net::packets;
                S2C_BattleResult pkt;
                pkt.runtimeCardId = card.runtimeId;
                pkt.dir = static_cast<uint8_t>(card.dir);
                pkt.ownerSlot = static_cast<uint8_t>(card.ownerSlot + 1);
                pkt.unitLocalIndex = static_cast<uint8_t>((unitIndex % 2) + 1);
                pkt.actionTime = 3000;
                pkt.order.push_back(buildDeltaSnapshot(unitIndex, eventOccurred));


                auto bytes = yuno::net::PacketBuilder::Build(
                    yuno::net::PacketType::S2C_BattleResult,
                    [&](yuno::net::ByteWriter& w)
                    {
                        pkt.Serialize(w);
                    });

                m_network.Broadcast(std::move(bytes));
            };

        constexpr size_t CardsPerPlayer = 4;

        if (slotCards[0].size() != CardsPerPlayer || slotCards[1].size() != CardsPerPlayer)
        {
            std::cout << "[Server] INVALID TURN CARD COUNT "
                << "p1=" << slotCards[0].size()
                << " p2=" << slotCards[1].size()
                << " expected=" << CardsPerPlayer
                << "\n";

            ClearTurn();
            return; // 잘못 들어왔으면 걍 턴종료

        }

        size_t maxCount = std::max(slotCards[0].size(), slotCards[1].size());

        for (size_t i = 0; i < maxCount; ++i)
        {

            bool coinTossUsed = false;
            bool aFirst = IsALess(slotCards[0][i], slotCards[1][i], coinTossUsed);

            if (coinTossUsed)
            {
                std::cout << "[Server] COIN TOSS EVENT "
                    << "A(runtime=" << slotCards[0][i].runtimeId << ") vs "
                    << "B(runtime=" << slotCards[1][i].runtimeId << ")\n";
            }

            if (aFirst)
            {
                applyCard(slotCards[0][i]);
                applyCard(slotCards[1][i]);
            }
            else
            {
                applyCard(slotCards[1][i]);
                applyCard(slotCards[0][i]);
            }

        }

    

        ClearTurn();
    }

    void TurnManager::ClearTurn()
    {
        m_turnCards[0].clear();
        m_turnCards[1].clear();
        m_submitted[0] = false;
        m_submitted[1] = false;
    }
}
