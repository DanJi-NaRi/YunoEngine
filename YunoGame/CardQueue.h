#pragma once
#include <vector>

using CardRuntimeId = uint32_t;

class CardQueue
{
public:
    static constexpr int MAX_CARD = 4;

    bool Push(CardRuntimeId id)
    {
        if (m_cards.size() >= MAX_CARD)
            return false;
        m_cards.push_back(id);
        return true;
    }

    void Clear()
    {
        m_cards.clear();
    }

    const std::vector<CardRuntimeId>& Get() const
    {
        return m_cards;
    }

    bool IsFull() const { return m_cards.size() >= MAX_CARD; }
    bool IsEmpty() const { return m_cards.empty(); }

private:
    std::vector<CardRuntimeId> m_cards;
};
