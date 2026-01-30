#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "ServerCardData.h"
#include "ServerCardActData.h"

namespace yuno::server
{
    class ServerCardManager
    {
    public:
        bool LoadFromCSV(const std::string& path);

        const std::unordered_map<int, CardData>&
            GetAllCards() const { return m_cardData; }

        const CardData& GetCardData(int cardID) const;
        const CardMoveData* GetMoveData(int cardID) const;
        const CardEffectData* GetEffectData(int cardID) const;
        const CardActData* GetActData(int cardID) const;

    private:
        std::unordered_map<int, CardData>       m_cardData;
        std::unordered_map<int, CardMoveData>   m_moveData;
        std::unordered_map<int, CardEffectData> m_effectData;
        std::unordered_map<int, CardActData>    m_actData;
    };
}
