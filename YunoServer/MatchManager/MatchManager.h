#pragma once

#include <array>
#include <cstdint>
#include <chrono>

namespace yuno::server
{
    // 무기 선택 페이즈때 필요한 데이터
    class MatchManager final
    {
    public:
        static constexpr std::size_t kMaxSlots = 2;

        struct MatchSlot
        {
            bool occupied = false;
            std::uint32_t userId = 0;        // 고정 ID (OS한테 받아올 예정)
            std::uint32_t sessionId = 0;    

            std::uint32_t unitId1 = 0;      // 선택 무기 1번 id
            std::uint32_t unitId2 = 0;      // 선택 무기 2번 id

            bool ready = false;

            bool IsReady() const { return (unitId1 != 0 && unitId2 != 0); }
            bool IsUnitsFilled() const
            {
                return unitId1 != 0 && unitId2 != 0;
            }

            std::chrono::steady_clock::time_point lastEmoteTime =
                std::chrono::steady_clock::time_point::min();
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

        std::array<MatchSlot, kMaxSlots>& Slots() { return m_slots; } // 쓰기 버전

        bool ClearUnitByUserId(std::uint32_t uid, int slotWeaponIndex);

        bool SetUnitsByUserId(std::uint32_t uid, std::uint32_t u1, std::uint32_t u2);

        bool SetReadyBySessionId(std::uint64_t sid, bool isReady);

        void InitBattleState();//battlestate 초기화

        void ClearReadyAndUnits();
    private:
        std::array<MatchSlot, kMaxSlots> m_slots{};
    };
}
// 이동 공격 까지 되면 굿 >> 카드랑 웨폰 데이터를 로드하는걸 만들어야돼
// 피0 >> 승패판정 
// 라운드시작 (Init)
// 루프가 돌아간다 
