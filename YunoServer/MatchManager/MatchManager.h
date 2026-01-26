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

    private:
        std::array<MatchSlot, kMaxSlots> m_slots{};
    };
}
