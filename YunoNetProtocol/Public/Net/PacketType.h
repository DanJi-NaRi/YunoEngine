#pragma once

#include <cstdint>

namespace yuno::net
{
    // 0000 0000 - 8bit 사용
    // 0xxx xxxx - B가 0 이면 C2S 패킷 1 ~ 127
    // 1xxx xxxx - (단 1111 xxxx 제외) B가 1 이면 S2C 패킷 128 ~ 239
    // 1111 xxxx - 상위 비트 4개가 1이면 System/Error 패킷 240 ~ 255
    enum class PacketType : std::uint8_t
    {
        Invalid = 0,

        // C2S
        C2S_MatchEnter = 1,   // 타이틀에서 매치 진입 요청
        C2S_MatchLeave = 2,   // 매치/로비 이탈
        C2S_ReadySet = 3,   // 준비 완료/해제 (isReady)
        C2S_SubmitWeapon = 4,   // 무기 선택 제출 (weaponId)
        C2S_SubmitCard = 5,   // 카드 제출 (cardId) <- 이거 안쓰고 있음
        C2S_Ping = 6,
        C2S_ReadyTurn = 7, //카드 선택 끝난 턴준비 보냄 <- 이게 카드 제출
        C2S_SelectCard = 8,
        // S2C
        S2C_EnterOK = 128,  // 매치 진입 승인
        S2C_ReadyState = 129,  // 양측 준비 상태 동기화
        S2C_CountDown = 130,  // 카운트다운 시작/동기화
        S2C_RoundStart = 131,  // 게임 시작
        S2C_BattleResult = 133,  // 전투 결과/상태 갱신
        S2C_StartCardList = 134, //카드id 배정
        S2C_DrawCandidates = 135, //카드추가후보3장 제시
        S2C_Pong = 136,

        // System / Error
        S2C_Error = 240, // 에러 코드/메시지
        S2C_Error_EnterDenied = 255
    };

    inline constexpr bool IsValidPacketType(PacketType t)
    {
        return t != PacketType::Invalid;
    }

    inline constexpr bool IsC2S(PacketType t)
    {
        return (static_cast<std::uint8_t>(t) & 0x80u) == 0u
            && t != PacketType::Invalid;
    }

    inline constexpr bool IsS2C(PacketType t)
    {
        const auto v = static_cast<std::uint8_t>(t);
        return (v & 0x80u) != 0u && (v & 0xF0u) != 0xF0u;
    }

    inline constexpr bool IsSystem(PacketType t)
    {
        return (static_cast<std::uint8_t>(t) & 0xF0u) == 0xF0u;
    }
}
namespace yuno::net::packets 
{
    enum class ErrorCode : std::uint16_t
    {
        None = 0,

        // Match/Room
        EnterDenied = 1,
    };

    enum class EnterDeniedReason : std::uint8_t
    {
        None = 0,
        RoomFull = 1,
        AlreadyInMatch = 2,
    };
}

