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

// 주어진 방향의 정반대 방향을 반환한다.
// None / Same은 방향성이 없으므로 그대로 돌려준다.
inline Direction GetOppositeDirection(Direction dir)
{
    switch (dir)
    {
    case Direction::Up:         return Direction::Down;
    case Direction::Down:       return Direction::Up;
    case Direction::Left:       return Direction::Right;
    case Direction::Right:      return Direction::Left;

    case Direction::UpLeft:     return Direction::DownRight;
    case Direction::UpRight:    return Direction::DownLeft;
    case Direction::DownLeft:   return Direction::UpRight;
    case Direction::DownRight:  return Direction::UpLeft;

    case Direction::None:
    case Direction::Same:
    default:                    return dir;
    }
}

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
