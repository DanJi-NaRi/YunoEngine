#pragma once
#include <vector>

#include "C2S_BattlePackets.h"

class CardQueue
{
public:
    static constexpr int MAX_CARD = 4;

    bool Push(const CardPlayCommand& cmd)
    {
        if (m_cards.size() >= MAX_CARD)
            return false;

        m_cards.push_back(cmd);
        return true;
    }

    void Clear()
    {
        m_cards.clear();
    }

    const std::vector<CardPlayCommand>& Get() const
    {
        return m_cards;
    }

    bool IsFull() const { return m_cards.size() >= MAX_CARD; }
    bool IsEmpty() const { return m_cards.empty(); }

private:
    std::vector<CardPlayCommand> m_cards;
};
