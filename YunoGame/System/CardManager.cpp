#include "pch.h"

#include "CardManager.h"


//CSV 한 줄 분리 유틸
static std::vector<std::string> Split(const std::string& line, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        tokens.push_back(item);
    }
    return tokens;
}

//BOM / 공백 제거 유틸
static void RemoveBOM(std::string& s)
{
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
    {
        s.erase(0, 3);
    }
}

//string -> wstring
static std::wstring Utf8ToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    int size_needed = MultiByteToWideChar(
        CP_UTF8, 0,
        str.c_str(), (int)str.size(),
        nullptr, 0
    );

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8, 0,
        str.c_str(), (int)str.size(),
        &wstr[0], size_needed
    );

    return wstr;
}

//CSV 로드 함수
bool CardManager::LoadFromCSV(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[CardManager] Failed to open CSV: " << path << std::endl;
        return false;
    }

    std::string line;

    // 첫 줄은 헤더이므로 스킵
    std::getline(file, line);

    while (std::getline(file, line))
    {
        //std::cout << "[CSV RAW] " << line << std::endl; 데이터 확인용
        if (line.empty())
            continue;

        auto cols = Split(line, ',');

        if (!cols.empty())
        {
            RemoveBOM(cols[0]);
        }

        // 최소 컬럼 수 방어
        if (cols.size() < 17)
            continue;
    
        // -------------------------
        // CardData
        // -------------------------
        CardData card;
        card.m_cardID = std::stoi(cols[0]);

        card.m_name = Utf8ToWString(cols[1]);

        card.m_allowedUnits = std::stoul(cols[2]);
        card.m_Rarity = std::stoi(cols[3]);
        card.m_type = static_cast<CardType>(std::stoi(cols[4]));
        card.m_speed = std::stoi(cols[5]);
        card.m_useId = std::stoi(cols[6]);
        card.m_cost = std::stoi(cols[7]);
        card.m_rangeId = std::stoul(cols[11]);
        card.m_controlId = std::stoul(cols[12]);
        card.m_destroyCnt = 0;

        card.m_effectId = 0;
        card.m_soundId = 0;
        card.m_explainText = L"";


        m_cardData[card.m_cardID] = card;

        // -------------------------
            // Move Data
            // -------------------------
        int moveX = std::stoi(cols[9]);
        int moveY = std::stoi(cols[10]);

        if (moveX != 0 || moveY != 0)
        {
            CardMoveData move;
            move.m_moveX = moveX;
            move.m_moveY = moveY;
            m_moveData[card.m_cardID] = move;
        }


        // -------------------------
            // Effect Data
            // -------------------------
        int damage = std::stoi(cols[8]);
        int giveDamageBonus = std::stoi(cols[13]);
        int takeDamageReduce = std::stoi(cols[14]);
        int takeDamageIncrease = std::stoi(cols[15]);
        int staminaRecover = std::stoi(cols[16]);

        // 이펙트 데이터가 다 0이어도 일단 다 넣는다.
        CardEffectData effect;
        effect.m_damage = damage;
        effect.m_giveDamageBonus = giveDamageBonus;
        effect.m_takeDamageReduce = takeDamageReduce;
        effect.m_takeDamageIncrease = takeDamageIncrease;
        effect.m_staminaRecover = staminaRecover;

        m_effectData[card.m_cardID] = effect;

        //if (damage != 0 || giveDamageBonus != 0 ||
        //    takeDamageReduce != 0 || takeDamageIncrease != 0 ||
        //    staminaRecover != 0)
        //{

        //}

        // =========================
        // Act Data (확장용, 지금은 선택)
        // =========================
        // 나중에 RangeType / CardUtility / Dir 필요해지면 여기서 파싱
    }

    std::cout << "[CardManager] Loaded Cards: "
        << m_cardData.size() << std::endl;

    return true;
}

//반환 함수
//참조 반환
const CardData& CardManager::GetCardData(int cardID) const
{
    auto it = m_cardData.find(cardID);
    if (it == m_cardData.end())
    {
        throw std::runtime_error("CardData not found");
    }
    return it->second;
}

//포인터 반환~
const CardMoveData* CardManager::GetMoveData(int cardID) const
{
    auto it = m_moveData.find(cardID);
    return (it != m_moveData.end()) ? &it->second : nullptr;
}

const CardEffectData* CardManager::GetEffectData(int cardID) const
{
    auto it = m_effectData.find(cardID);
    return (it != m_effectData.end()) ? &it->second : nullptr;
}

const std::wstring CardManager::GetCardTexturePath(int cardID) const
{
    std::wstring cardPath = L"../Assets/UI/CARD/card_" + GetCardData(cardID).m_name + L".png";
    return cardPath;
}

