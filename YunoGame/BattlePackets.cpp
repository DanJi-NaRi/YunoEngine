#include "pch.h"
#include "BattlePackets.h"

Dirty_US Diff_US(const UnitState& prev, const UnitState& cur)
{
    Dirty_US d = Dirty_US::None;
    if (prev.pId != cur.pId)                        d = d | Dirty_US::pId;
    if (prev.slotId != cur.slotId)                  d = d | Dirty_US::slotId;
    if (prev.hp != cur.hp)                          d = d | Dirty_US::hp;
    if (prev.stamina != cur.stamina)                d = d | Dirty_US::stamina;
    if (prev.targetTileID != cur.targetTileID)      d = d | Dirty_US::targetTileID;
    if (prev.isEvent != cur.isEvent)                d = d | Dirty_US::isEvent;
    return d;
}
