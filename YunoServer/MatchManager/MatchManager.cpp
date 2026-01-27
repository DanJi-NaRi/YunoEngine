#include "MatchManager.h"

namespace yuno::server
{
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

        m_slots[idx].occupied = false;
        m_slots[idx].userId = 0;
        m_slots[idx].sessionId = 0;
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
        return false;
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
}
