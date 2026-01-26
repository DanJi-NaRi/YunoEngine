#pragma once

#include <array>
#include <cstdint>

namespace yuno::server
{
    class MatchManager final
    {
    public:
        static constexpr std::size_t kMaxSlots = 2;

        struct MatchSlot
        {
            bool occupied = false;
            std::uint32_t userId = 0;        // 고정 ID (OS한테 받아올 예정)
            std::uint64_t sessionId = 0; 

            std::uint32_t unitId1 = 0;
            std::uint32_t unitId2 = 0;

            bool ready = false;

            bool IsReady() const { return (unitId1 != 0 && unitId2 != 0); }
            bool IsUnitsFilled() const
            {
                return unitId1 != 0 && unitId2 != 0;
            }
        };
    public:
        MatchManager() = default;

        int FindSlotBySessionId(std::uint64_t sid) const;
        int FindSlotByUserId(std::uint32_t uid) const;

        int AllocateOrAttachUser(std::uint32_t uid, std::uint64_t sid);

        void DetachBySessionId(std::uint64_t sid);

        void LeaveByUserId(std::uint32_t uid);

        std::uint8_t GetOccupiedCount() const;

        const std::array<MatchSlot, kMaxSlots>& Slots() const { return m_slots; }

        bool SetUnitByUserId(std::uint32_t uid, int slotWeaponIndex, std::uint32_t unitId);

        bool ClearUnitByUserId(std::uint32_t uid, int slotWeaponIndex);

        bool SetUnitsByUserId(std::uint32_t uid, std::uint32_t u1, std::uint32_t u2);

        bool SetReadyBySessionId(std::uint64_t sid, bool isReady);

    private:
        std::array<MatchSlot, kMaxSlots> m_slots{};
    };
}
