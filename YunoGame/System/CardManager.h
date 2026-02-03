#pragma once

#include "CardData.h"

class CardManager
{
public:
    bool LoadFromCSV(const std::string& path);

    const CardData& GetCardData(int cardID) const;
    const CardMoveData* GetMoveData(int cardID) const;
    const CardEffectData* GetEffectData(int cardID) const;

private:
    std::unordered_map<int, CardData>       m_cardData;
    std::unordered_map<int, CardMoveData>   m_moveData;
    std::unordered_map<int, CardEffectData> m_effectData;
};
