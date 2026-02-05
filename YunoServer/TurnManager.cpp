#include <iostream>
#include <array>
#include <algorithm>
#include <cstdlib>
#include <limits>

#include "TurnManager.h"
#include "MatchManager.h"
#include "YunoServerNetwork.h"
#include "RoundController.h"

#include "PacketBuilder.h"
#include "S2C_BattlePackets.h"
#include "S2C_CardPackets.h"
#include "ServerCardManager.h"
#include "ServerCardRangeManager.h"

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

        TilePos RotateRangeOffset(const RangeOffset& offset, Direction dir)
        {
            switch (dir)
            {
            case Direction::Up:
                return { -offset.dy , -offset.dx};
            case Direction::Down:
                return { offset.dy, offset.dx };
            case Direction::Left:
                return { -offset.dx, -offset.dy };
            case Direction::Right: // 얘가 디폴트 방향임 (1,1)
                return { offset.dx, -offset.dy };
            default:
                return { offset.dx, -offset.dy };
            }
        }
    }

    TurnManager::TurnManager(
        MatchManager& match,
        YunoServerNetwork& network,
        ServerCardRuntime& runtime,
        ServerCardManager& cardDB,
        ServerCardRangeManager& rangeDB,
        RoundController& roundController)
        : m_match(match)
        , m_network(network)
        , m_runtime(runtime)
        , m_cardDB(cardDB)
        , m_cardRangeDB(rangeDB)
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

        std::array<BuffState, 4> buffStates{};

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

        auto isPlayerAllDead = [&](int playerIndex) -> bool
            {
                const UnitState& u1 = *units[playerIndex * 2];
                const UnitState& u2 = *units[playerIndex * 2 + 1];
                return u1.hp == 0 && u2.hp == 0;
            };

        auto evaluateRoundEnd = [&]() -> bool
            {
                bool p1AllDead = isPlayerAllDead(0);
                bool p2AllDead = isPlayerAllDead(1);

                if (!p1AllDead && !p2AllDead)
                    return false;

                g_battleState.roundEnded = true;

                if (p1AllDead && !p2AllDead)
                {
                    g_battleState.roundWinnerPID = 2;
                    g_battleState.roundLoserPID = 1;
                    g_battleState.roundWins[1]++;
                }
                else if (!p1AllDead && p2AllDead)
                {
                    g_battleState.roundWinnerPID = 1;
                    g_battleState.roundLoserPID = 2;
                    g_battleState.roundWins[0]++;
                }
                else
                {
                    // 동시 전멸은 무승부 처리
                    g_battleState.roundWinnerPID = 0;
                    g_battleState.roundLoserPID = 0;
                }

                if (g_battleState.roundWins[0] >= g_battleState.winsToFinish)
                {
                    g_battleState.matchEnded = true;
                    g_battleState.matchWinnerPID = 1;
                }
                else if (g_battleState.roundWins[1] >= g_battleState.winsToFinish)
                {
                    g_battleState.matchEnded = true;
                    g_battleState.matchWinnerPID = 2;
                }

                if (!g_battleState.matchEnded)
                {
                    g_battleState.currentRound = static_cast<uint8_t>(g_battleState.currentRound + 1);
                }

                std::cout << "[Server] Round ended. winnerPID="
                    << static_cast<int>(g_battleState.roundWinnerPID)
                    << " loserPID=" << static_cast<int>(g_battleState.roundLoserPID)
                    << " | score P1=" << static_cast<int>(g_battleState.roundWins[0])
                    << " P2=" << static_cast<int>(g_battleState.roundWins[1])
                    << " | matchEnded=" << (g_battleState.matchEnded ? 1 : 0)
                    << " matchWinnerPID=" << static_cast<int>(g_battleState.matchWinnerPID)
                    << "\n";

                return true;
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
                    int occupantIndex = grid[probeId];
                    if (occupantIndex != -1) // 무언가가 있는경우
                    {
                        std::cout << "Blocked by unit on tile " << static_cast<int>(probeId) << std::endl;
                        UnitState& other = *units[occupantIndex];
                        bool isEnemy = (unitIndex / 2) != (occupantIndex / 2);

                        if (isEnemy)
                        {
                            // 적군 충돌
                            std::cout << "Enemy collision: self -10, enemy -5" << std::endl;
                            
                            auto subClamp = [](auto& hp, int dmg)
                                {
                                    using T = std::decay_t<decltype(hp)>;
                                    int v = static_cast<int>(hp) - dmg;
                                    if (v < 0) v = 0;
                                    hp = static_cast<T>(v);
                                };

                            subClamp(unit.hp, 10);
                            subClamp(other.hp, 5);
                        }
                        else
                        {
                            // 아군 충돌
                            std::cout << "Blocked by ally" << std::endl;
                        }

                        return true; // 이동은 항상 막힘
                    }
                }

                uint8_t oldTile = unit.tileID;
                uint8_t newTile = PosToTileId(target);
                grid[oldTile] = -1;
                grid[newTile] = unitIndex;
                unit.tileID = newTile;
                std::cout << "oldPos : " << static_cast<int>(oldTile) << ", newPos : " << static_cast<int>(newTile) << std::endl;
                return false;
            };

        auto addClamp = [](auto& value, int delta)
            {
                using T = std::decay_t<decltype(value)>;
                int v = static_cast<int>(value) + delta;
                int maxValue = static_cast<int>(std::numeric_limits<T>::max());
                if (v < 0) v = 0;
                if (v > maxValue) v = maxValue;
                value = static_cast<T>(v);
            };

        auto applyDisplacement = [&](int targetIndex, const TilePos& step, int distance) -> bool
            {
                if (step.x == 0 && step.y == 0)
                    return false;

                UnitState& targetUnit = *units[targetIndex];
                if (targetUnit.tileID == 0)
                    return false;

                bool moved = false;
                TilePos current = TileIdToPos(targetUnit.tileID);

                for (int i = 0; i < distance; ++i)
                {
                    TilePos next = { current.x + step.x, current.y + step.y };
                    if (!IsInBounds(next))
                    {
                        std::cout << "Out of Grid" << std::endl;
                        break;
                    }

                    uint8_t nextTile = PosToTileId(next);
                    if (grid[nextTile] != -1)
                    {
                        std::cout << "object exist" << std::endl;
                        break;
                    }

                    grid[targetUnit.tileID] = -1;
                    grid[nextTile] = targetIndex;
                    targetUnit.tileID = nextTile;
                    current = next;
                    moved = true;
                }

                return moved;
            };

        auto applyAttack = [&](int unitIndex, uint32_t cardDataId, const CardData& cardData, Direction dir, std::vector<int>* hitTargets = nullptr) -> bool
            {
                const auto* effectData = m_cardDB.GetEffectData(cardDataId);
                const auto* rangeData = m_cardRangeDB.GetRange(cardData.m_rangeId);

                if (!effectData)
                {
                    std::cout << "Attack effect data missing" << std::endl;
                    return false;
                }
                if (!rangeData)
                {
                    std::cout << "Attack range data missing" << std::endl;
                    return false;
                }

                UnitState& attacker = *units[unitIndex];
                if (attacker.tileID == 0)
                    return false;

                TilePos origin = TileIdToPos(attacker.tileID);
                int totalDamage = effectData->m_damage;

                if (attacker.buffstat.nextDamageBonus != 0)
                {
                    std::cout << "--Damage Enhanced-- ";
                    totalDamage += attacker.buffstat.nextDamageBonus;
                    attacker.buffstat.nextDamageBonus = 0;
                }

                auto subClamp = [](auto& hp, int dmg)
                    {
                        using T = std::decay_t<decltype(hp)>;
                        int v = static_cast<int>(hp) - dmg;
                        if (v < 0) v = 0;
                        hp = static_cast<T>(v);
                    };

                bool eventOccurred = false;

                for (const auto& offset : rangeData->offsets)
                {
                    TilePos delta = RotateRangeOffset(offset, dir);
                    TilePos target = { origin.x + delta.x, origin.y + delta.y };

                    if (!IsInBounds(target))
                    {
                        std::cout << "Enter Out of Grid" << std::endl;
                        continue;
                    }

                    uint8_t targetTile = PosToTileId(target);
                    int occupantIndex = grid[targetTile];

                    if (occupantIndex == -1)
                    {
                        std::cout << "Enter Empty tile" << std::endl;
                        continue;
                    }

                    bool isEnemy = (unitIndex / 2) != (occupantIndex / 2);
                    if (!isEnemy)
                    {
                        std::cout << "Enter Ally Attack" << std::endl;
                        continue;
                    }

                    if (hitTargets)
                    {
                        if (std::find(hitTargets->begin(), hitTargets->end(), occupantIndex) == hitTargets->end())
                        {
                            hitTargets->push_back(occupantIndex);
                        }
                    }

                    if (totalDamage > 0)
                    {
                        UnitState& targetUnit = *units[occupantIndex];
                        int adjustedDamage = totalDamage;
                        if (targetUnit.buffstat.nextDamageIncrease != 0) // 타겟이 보유중인 받는 피해 증가 디버프가 있는 경우
                        {
                            adjustedDamage += targetUnit.buffstat.nextDamageIncrease;
                            targetUnit.buffstat.nextDamageIncrease = 0;
                        }
                        if (targetUnit.buffstat.nextDamageReduce != 0)  // 타겟이 보유중인 받는 피해 감소 버프 있는 경우
                        {
                            adjustedDamage = std::max(0, adjustedDamage - targetUnit.buffstat.nextDamageReduce);
                            targetUnit.buffstat.nextDamageReduce = 0;
                        }

                        subClamp(targetUnit.hp, adjustedDamage);

                        eventOccurred = true;

                        std::cout << "Attack hit unit " << occupantIndex
                            << " damage=" << adjustedDamage << std::endl;
                    }
                }

                return eventOccurred;
            };

        auto applyCard = [&](const ResolvedCard& card)
            {
				static int count = 0;


                const CardData& cardData = m_cardDB.GetCardData(card.dataId);
                int unitIndex = getUnitIndexForCard(card.ownerSlot, cardData);
                bool eventOccurred = false;
                bool utilityMoveOccurred = false;
                bool utilityAttackOccurred = false;
                bool utilityControlOccurred = false;
                std::array<yuno::net::packets::UnitStateDelta, 4> utilityMoveSnapshot{};
                std::array<yuno::net::packets::UnitStateDelta, 4> utilityAttackSnapshot{};
                std::array<yuno::net::packets::UnitStateDelta, 4> utilityControlSnapshot{};
                
                std::cout << "-----------Unit Index : " << unitIndex << "  " << "applyCard Count : " << ++count << std::endl;

                UnitState& unit = *units[unitIndex];

                if (unit.hp == 0)
                {
                    grid[unit.tileID] = -1;
                    std::cout << "[Server] Skip card(runtime=" << card.runtimeId
                        << ") because unit is dead. unitIndex=" << unitIndex << "\n";

                    using namespace yuno::net::packets;
                    S2C_BattleResult pkt;
                    pkt.runtimeCardId = card.runtimeId;
                    pkt.dir = static_cast<uint8_t>(card.dir);
                    pkt.ownerSlot = static_cast<uint8_t>(card.ownerSlot + 1);
                    pkt.unitLocalIndex = static_cast<uint8_t>((unitIndex % 2) + 1);
                    pkt.actionTime = 3000;
                    if (cardData.m_type == CardType::Utility)
                    {
                        pkt.order.push_back(buildDeltaSnapshot(unitIndex, false));
                        pkt.order.push_back(buildDeltaSnapshot(unitIndex, false));
                        pkt.order.push_back(buildDeltaSnapshot(unitIndex, false));
                    }
                    else
                    {
                        pkt.order.push_back(buildDeltaSnapshot(unitIndex, false));
                    }

                    auto bytes = yuno::net::PacketBuilder::Build(
                        yuno::net::PacketType::S2C_BattleResult,
                        [&](yuno::net::ByteWriter& w)
                        {
                            pkt.Serialize(w);
                        });

                    m_network.Broadcast(std::move(bytes));
                    return;
                }

                if (g_battleState.roundEnded)
                {
                    return;
                }

                
                if (cardData.m_cost > 0)
                {
                    UnitState& unit = *units[unitIndex];
                    std::cout << "before stamina : " << static_cast<int>(unit.stamina);
                    unit.stamina = static_cast<uint8_t>(
                        std::max(0, static_cast<int>(unit.stamina) - cardData.m_cost));
                    std::cout << ", after stamina : " << static_cast<int>(unit.stamina) << std::endl;
                }
                else 
                {
                    std::cout << "do not Use Cost" << std::endl;
                }

                if (cardData.m_type == CardType::Buff)
                {
                    std::cout << "Buff Card On" << std::endl;
                    if (const auto* effectData = m_cardDB.GetEffectData(card.dataId))
                    {
                        UnitState& unit = *units[unitIndex];
                        if (effectData->m_staminaRecover != 0)
                        {
                            std::cout << "--Stamana Recovery--" << std::endl;
                            addClamp(unit.stamina, effectData->m_staminaRecover);
                        }
                        if (effectData->m_giveDamageBonus != 0)
                        {
                            std::cout << "--Get nextDamageBonus--" << std::endl;
                            unit.buffstat.nextDamageBonus += effectData->m_giveDamageBonus;
                        }
                        if (effectData->m_takeDamageReduce != 0)
                        {
                            std::cout << "--Get nextDamageReduce--" << std::endl;
                            unit.buffstat.nextDamageReduce += effectData->m_takeDamageReduce;
                        }
                        // 얘는 특수였음;;  
/*                      if (effectData->m_takeDamageIncrease != 0)
                        {
                            std::cout << "--Stamana Recovery--" << std::endl;
                            unit.buffstat.nextDamageIncrease += effectData->m_takeDamageIncrease;
                        }*/
                    }
                }
                else if(cardData.m_type == CardType::Move)
                {
                    if (const auto* moveData = m_cardDB.GetMoveData(card.dataId))
                    {
                        eventOccurred = applyMove(unitIndex, *moveData, card.dir);
                    }
                }
                else if (cardData.m_type == CardType::Attack)
                {
                    std::cout << "Attck Card On" << std::endl;
                    applyAttack(unitIndex, card.dataId, cardData, card.dir);
					
                }
                else if (cardData.m_type == CardType::Utility)
                {
                    std::cout << "Utility Card On" << std::endl;

                    UnitState& unit = *units[unitIndex];
                    if (const auto* moveData = m_cardDB.GetMoveData(card.dataId))
                    {
                        utilityMoveOccurred = applyMove(unitIndex, *moveData, card.dir);
                        eventOccurred = utilityMoveOccurred || eventOccurred;
                    }
                
                    utilityMoveSnapshot = buildDeltaSnapshot(unitIndex, utilityMoveOccurred);

                    const auto* effectData = m_cardDB.GetEffectData(card.dataId);
                    std::vector<int> hitTargets;

                    if (effectData && effectData->m_giveDamageBonus != 0 && effectData->m_damage == 0)
                    {
                        std::cout << "--Get nextDamageBonus--" << std::endl;
                        unit.buffstat.nextDamageBonus += effectData->m_giveDamageBonus;
                    }

                    if (units[unitIndex]->hp > 0 && effectData && effectData->m_damage != 0)
                    {
                        utilityAttackOccurred = applyAttack(unitIndex, card.dataId, cardData, card.dir, &hitTargets);
                        eventOccurred = utilityAttackOccurred || eventOccurred;
                    }

                    if (effectData && effectData->m_takeDamageIncrease != 0 && !hitTargets.empty())
                    {
                        for (int targetIndex : hitTargets)
                        {
                            UnitState& targetUnit = *units[targetIndex];
                            targetUnit.buffstat.nextDamageIncrease += effectData->m_takeDamageIncrease;
                        }
                    }
                    utilityAttackSnapshot = buildDeltaSnapshot(unitIndex, utilityAttackOccurred);

                    if (cardData.m_controlId != 0 && !hitTargets.empty())
                    {
                        TilePos pushStep = { 0, 0 };
                        TilePos pullStep = { 0, 0 };
                        switch (card.dir)
                        {
                        case Direction::Up:
                            pushStep = { 0, -1 };
                            pullStep = { 0, 1 };
                            break;
                        case Direction::Down:
                            pushStep = { 0, 1 };
                            pullStep = { 0, -1 };
                            break;
                        case Direction::Left:
                            pushStep = { -1, 0 };
                            pullStep = { 1, 0 };
                            break;
                        case Direction::Right:
                        default:
                            pushStep = { 1, 0 };
                            pullStep = { -1, 0 };
                            break;
                        }

                        for (int targetIndex : hitTargets)
                        {
                            if (units[targetIndex]->hp == 0)
                            {
                                grid[units[targetIndex]->slotID] = -1;
                                continue;
                            }
                            if (cardData.m_controlId == 1)
                            {
                                bool displaced = applyDisplacement(targetIndex, pullStep, 1);
                                utilityControlOccurred = displaced || utilityControlOccurred;
                                eventOccurred = displaced || eventOccurred;
                            }
                            else if (cardData.m_controlId == 2)
                            {
                                bool displaced = applyDisplacement(targetIndex, pushStep, 1);
                                utilityControlOccurred = displaced || utilityControlOccurred;
                                eventOccurred = displaced || eventOccurred;
                            }
                        }
                    }
                    utilityControlSnapshot = buildDeltaSnapshot(unitIndex, utilityControlOccurred);
                }
                else { std::cout << "Card Type is invalid" << std::endl; };
                using namespace yuno::net::packets;
                S2C_BattleResult pkt;
                pkt.runtimeCardId = card.runtimeId;
                pkt.dir = static_cast<uint8_t>(card.dir);
                pkt.ownerSlot = static_cast<uint8_t>(card.ownerSlot + 1);
                pkt.unitLocalIndex = static_cast<uint8_t>((unitIndex % 2) + 1);
                pkt.actionTime = 3000;
                if (cardData.m_type == CardType::Utility)
                {
                    pkt.order.push_back(utilityMoveSnapshot);
                    pkt.order.push_back(utilityAttackSnapshot);
                    pkt.order.push_back(utilityControlSnapshot);
                }
                else
                {
                    pkt.order.push_back(buildDeltaSnapshot(unitIndex, eventOccurred));
                }


                auto bytes = yuno::net::PacketBuilder::Build(
                    yuno::net::PacketType::S2C_BattleResult,
                    [&](yuno::net::ByteWriter& w)
                    {
                        pkt.Serialize(w);
                    });

                m_network.Broadcast(std::move(bytes));
                evaluateRoundEnd();
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
                if (g_battleState.roundEnded) break;
                applyCard(slotCards[1][i]);
            }
            else
            {
                applyCard(slotCards[1][i]);
                if (g_battleState.roundEnded) break;
                applyCard(slotCards[0][i]);
            }

            if (g_battleState.roundEnded)
            {
                break;
            }


        }
        ClearTurn();
        NotifyEndFinished();
    }

    void TurnManager::ClearTurn()
    {
        m_turnCards[0].clear();
        m_turnCards[1].clear();
        m_submitted[0] = false;
        m_submitted[1] = false;
    }
    void TurnManager::NotifyEndFinished()
    {
        m_roundController.EndTurn();
    }
}
