#pragma once

using namespace yuno::server;

struct CardActionContext
{
    uint32_t runtimeID;
    uint32_t dataID;
    int ownerSlot;
    Direction dir;

    const CardData& card;     // CSV에서 읽은 카드 데이터
    BattleState& battle;     // 현재 전투 상태 (유닛 위치, HP, 스태미나 등)
};
