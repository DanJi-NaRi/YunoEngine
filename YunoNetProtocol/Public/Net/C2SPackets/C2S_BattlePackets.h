#pragma once

enum class Direction : uint8_t
{
    None,
    Up,
    Down,
    Left,
    Right,

    UpLeft,
    UpRight,
    DownLeft,
    DownRight,

    Same
};

struct CardPlayCommand
{
    uint32_t runtimeID;   // 어떤 카드인가
    Direction dir;             // 플레이어가 지정한 방향
};

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct C2S_ReadyTurn final
    {

        std::vector<CardPlayCommand> commands;

        void Serialize(ByteWriter& w) const;
        static C2S_ReadyTurn Deserialize(ByteReader& r);
    };


    struct C2S_RoundStartReadyOK final
    {

        void Serialize(ByteWriter& w) const;
        static C2S_RoundStartReadyOK Deserialize(ByteReader& r);
    };
}
