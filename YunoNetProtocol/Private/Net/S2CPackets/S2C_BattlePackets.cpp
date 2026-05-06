#include "pch.h"

#include "S2C_BattlePackets.h"

namespace yuno::net::packets
{
    // UnitStateDelta
    void UnitStateDelta::Serialize(ByteWriter& w) const
    {
        w.WriteU8(ownerSlot);
        w.WriteU8(unitLocalIndex);

        w.WriteU8(hp);
        w.WriteU8(stamina);
        w.WriteU8(targetTileID);
        w.WriteU8(isEvent);

    }

    UnitStateDelta UnitStateDelta::Deserialize(ByteReader& r)
    {
        UnitStateDelta d;
        d.ownerSlot = r.ReadU8();
        d.unitLocalIndex = r.ReadU8();

        d.hp = r.ReadU8();
        d.stamina = r.ReadU8();
        d.targetTileID = r.ReadU8();
        d.isEvent = r.ReadU8();
        return d;
    }

    //ObstacleState
    void ObstacleState::Serialize(ByteWriter& w) const
    {
        w.WriteU8(obstacleID);
        w.WriteU32LE(actionTime);
        w.WriteU8(static_cast<uint8_t>(tileIDs.size()));

        for (uint8_t tileId : tileIDs)
        {
            w.WriteU8(tileId);
        }
        for (const auto& u : unitState)
        {
            u.Serialize(w);
        }
    }

    ObstacleState ObstacleState::Deserialize(ByteReader& r)
    {
        ObstacleState o;

        o.obstacleID = r.ReadU8();
        o.actionTime = r.ReadU32LE();
        uint8_t tileCount = r.ReadU8();
        o.tileIDs.reserve(tileCount);

        for (uint8_t i = 0; i < tileCount; ++i)
        {
            o.tileIDs.push_back(r.ReadU8());
        }
        for (auto& u : o.unitState)
        {
            u = UnitStateDelta::Deserialize(r);
        }

        return o;
    }

    // S2C_BattleResult
    void S2C_BattleResult::Serialize(ByteWriter& w) const
    {
        w.WriteU32LE(runtimeCardId);
        w.WriteU8(dir);
        w.WriteU8(ownerSlot);
        w.WriteU8(unitLocalIndex);
        w.WriteU8(isCoinTossUsed);
        w.WriteU32LE(actionTime);

        // order count
        w.WriteU8(order.size());

        for (const auto& d : order)
        {
            d[0].Serialize(w);
            d[1].Serialize(w);
            d[2].Serialize(w);
            d[3].Serialize(w);
        }
    }

    S2C_BattleResult S2C_BattleResult::Deserialize(ByteReader& r)
    {
        S2C_BattleResult pkt;
        pkt.runtimeCardId = r.ReadU32LE();
        pkt.dir = r.ReadU8();
        pkt.ownerSlot = r.ReadU8();
        pkt.unitLocalIndex = r.ReadU8();
        pkt.isCoinTossUsed = r.ReadU8();
        pkt.actionTime = r.ReadU32LE();

        uint8_t orderSize = r.ReadU8();

        //uint8_t count = r.ReadU8();
        pkt.order.reserve(orderSize);

        for (uint8_t i = 0; i < orderSize; ++i)
        {
            std::array<UnitStateDelta, 4> us;
            us[0] = UnitStateDelta::Deserialize(r);
            us[1] = UnitStateDelta::Deserialize(r);
            us[2] = UnitStateDelta::Deserialize(r);
            us[3] = UnitStateDelta::Deserialize(r);
            pkt.order.push_back(us);
        }

        return pkt;
    }

    //S2C_ObstacleResult
    void S2C_ObstacleResult::Serialize(ByteWriter& w) const
    {
        w.WriteU8(static_cast<uint8_t>(obstacles.size()));
        for (const auto& o : obstacles)
        {
            o.Serialize(w);
        }
    }

    S2C_ObstacleResult S2C_ObstacleResult::Deserialize(ByteReader& r)
    {
        S2C_ObstacleResult pkt;
        uint8_t count = r.ReadU8();
        pkt.obstacles.reserve(count);

        for (uint8_t i = 0; i < count; ++i)
        {
            pkt.obstacles.push_back(ObstacleState::Deserialize(r));
        }
        return pkt;
    }
}
