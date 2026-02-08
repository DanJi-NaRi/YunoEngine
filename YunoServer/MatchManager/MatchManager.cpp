#include "MatchManager.h"

#include "ServerCardManager.h"
#include "S2C_CardPackets.h"
#include "BattleState.h"

namespace yuno::server
{
    BattleState g_battleState{};

    namespace
    {
        void ResetSlot(MatchManager::MatchSlot& slot)
        {
            slot.occupied = false;
            slot.userId = 0;
            slot.sessionId = 0;
            slot.unitId1 = 0;
            slot.unitId2 = 0;
            slot.ready = false;
        }
    }


    int MatchManager::FindSlotBySessionId(std::uint64_t sid) const
    {
        if (sid == 0) return -1;

        for (std::size_t i = 0; i < m_slots.size(); ++i)
        {
            if (m_slots[i].occupied && m_slots[i].sessionId == sid)
                return static_cast<int>(i);
        }
        return -1;
    }

    int MatchManager::FindSlotByUserId(std::uint32_t uid) const
    {
        if (uid == 0) return -1;

        for (std::size_t i = 0; i < m_slots.size(); ++i)
        {
            if (m_slots[i].occupied && m_slots[i].userId == uid)
                return static_cast<int>(i);
        }
        return -1;
    }

    int MatchManager::AllocateOrAttachUser(std::uint32_t uid, std::uint64_t sid)
    {
        if (uid == 0 || sid == 0) return -1;

        const int existing = FindSlotByUserId(uid);
        if (existing >= 0)
        {
            m_slots[existing].sessionId = sid;
            return existing;
        }

        for (std::size_t i = 0; i < m_slots.size(); ++i)
        {
            if (!m_slots[i].occupied)
            {
                ResetSlot(m_slots[i]);

                m_slots[i].occupied = true;
                m_slots[i].userId = uid;
                m_slots[i].sessionId = sid;
                return static_cast<int>(i);
            }
        }

        // full
        return -1;
    }

    void MatchManager::DetachBySessionId(std::uint64_t sid)
    {
        const int idx = FindSlotBySessionId(sid);
        if (idx < 0) return;

        m_slots[idx].sessionId = 0;
    }

    void MatchManager::LeaveByUserId(std::uint32_t uid)
    {
        const int idx = FindSlotByUserId(uid);
        if (idx < 0) return;

        ResetSlot(m_slots[idx]);
    }

    std::uint8_t MatchManager::GetOccupiedCount() const
    {
        std::uint8_t c = 0;
        for (const auto& s : m_slots)
        {
            if (s.occupied) ++c;
        }
        return c;
    }

    bool MatchManager::ClearUnitByUserId(std::uint32_t uid, int slotWeaponIndex)
    {
        const int idx = FindSlotByUserId(uid);
        if (idx < 0)
            return false;

        auto& slot = m_slots[static_cast<std::size_t>(idx)];
        if (!slot.occupied)
            return false;

        if (slotWeaponIndex == 1)
            slot.unitId1 = 0;
        else if (slotWeaponIndex == 2)
            slot.unitId2 = 0;
        else
            return false;

        slot.ready = false;
        return true;
    }

    bool MatchManager::SetUnitsByUserId(std::uint32_t uid, std::uint32_t u1, std::uint32_t u2)
    {
        const int idx = FindSlotByUserId(uid);
        if (idx < 0)
            return false;

        auto& slot = m_slots[static_cast<std::size_t>(idx)];
        if (!slot.occupied)
            return false;

        slot.unitId1 = u1;
        slot.unitId2 = u2;
        return true;
    }


    bool MatchManager::SetReadyBySessionId(std::uint64_t sid, bool isReady)
    {
        const int idx = FindSlotBySessionId(sid);
        if (idx < 0) return false;

        auto& slot = m_slots[static_cast<std::size_t>(idx)];
        if (!slot.occupied) return false;

        slot.ready = isReady;
        return true;
    }

    constexpr int kGridColumns = 7;

    static uint8_t PosToTileId(int x, int y)
    {
        return static_cast<uint8_t>(y * kGridColumns + x + 1);
    }
    static void InitUnitPosition(UnitState& unit, int x, int y)
    {
        unit.x = x;
        unit.y = y;
        unit.tileID = PosToTileId(x, y);
    }
    static void InitUnitStatsByUnitId(UnitState& unit)
    {
        switch (unit.WeaponID)
        {
        case 1: // 블래스터
            unit.hp = 80;
            unit.stamina = 120;
            break;

        case 2: // 차크람
            unit.hp = 90;
            unit.stamina = 110;
            break;

        case 3: // 브리처  
            unit.hp = 100;
            unit.stamina = 100;
            break;

        case 4: // 사이드
            unit.hp = 100;
            unit.stamina = 100;
            break;

        case 5: // 임팩터
            unit.hp = 120;
            unit.stamina = 80;
            break;             

        case 6: // 클리버
            unit.hp = 110;
            unit.stamina = 90;
            break;

        default:
            unit.hp = 100;
            unit.stamina = 100;
            break;
        }
    }
    void MatchManager::InitBattleState()
    {
        g_battleState.turnNumber = 0;
        g_battleState.roundEnded = false;
        g_battleState.roundWinnerPID = 0;
        g_battleState.roundLoserPID = 0;

        for (int i = 0; i < 2; ++i)
        {
            const auto& slot = m_slots[i];
            auto& player = g_battleState.players[i];

            // 기본 정보
            player.sessionId = slot.sessionId;
            player.PID = static_cast<uint8_t>(i+1);       // 이거는 환경변수로 초기화 하면 안됨

            //UnitSlot 고정
            player.unit1.slotID = 1;
            player.unit2.slotID = 2;

            // UnitID 초기화
            player.unit1.WeaponID = slot.unitId1;
            player.unit2.WeaponID = slot.unitId2;


            // Unit BuffStat 초기화
            player.unit1.buffstat = { 0,0,0 };
            player.unit2.buffstat = { 0,0,0 };

            //유닛 위치 초기화
            if (i == 0)
            {
                InitUnitPosition(player.unit1, 1, 1);
                InitUnitPosition(player.unit2, 1, 3);
            }
            else
            {
                InitUnitPosition(player.unit1, 5, 1);
                InitUnitPosition(player.unit2, 5, 3);
            }

            InitUnitStatsByUnitId(player.unit1);
            InitUnitStatsByUnitId(player.unit2);
        }
    }

    void MatchManager::ClearReadyAndUnits()
    {
        for (auto& s : m_slots)
        {
            s.ready = false;
            s.unitId1 = 0;
            s.unitId2 = 0;
        }
    }
}
