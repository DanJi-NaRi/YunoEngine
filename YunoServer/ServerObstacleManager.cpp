#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "ServerObstacleManager.h"

namespace yuno::server
{
    // --------------------------------
    // CSV 유틸 (카드 매니저랑 동일)
    // --------------------------------
    static std::vector<std::string> Split(const std::string& line, char delim)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string item;

        while (std::getline(ss, item, delim))
            tokens.push_back(item);

        return tokens;
    }

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

    // --------------------------------
    // CSV 로드
    // --------------------------------
    bool ServerObstacleManager::LoadFromCSV(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "[ObstacleManager] Failed to open CSV: "
                << path << std::endl;
            return false;
        }

        std::string line;

        // 헤더 스킵
        std::getline(file, line);

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            auto cols = Split(line, ',');
            if (cols.size() < 5)
                continue;

            RemoveBOM(cols[0]);

            ServerObstacleData data;
            data.roundId = std::stoi(cols[0]);
            data.turnId = std::stoi(cols[1]);
            data.obstacleType = std::stoi(cols[3]);
            data.damage = std::stoi(cols[4]);

            // tileIds 파싱 ("3;4;5")
            auto tiles = Split(cols[2], ';'); // 이거 csv에 2로 되어있어서 바꿈
            for (const auto& t : tiles)
            {
                if (t.empty())
                    continue;

                const int tileId = std::stoi(t);
                if (tileId <= 0)
                    continue;

                data.tileIds.push_back(tileId);
            }

            int key = data.roundId * 100 + data.turnId;
            m_ObstacleData[key].push_back(std::move(data));
        }

        std::cout << "[ServerObstacleManager] Loaded obstacle entries: "
            << m_ObstacleData.size() << std::endl;

        return true;
    }

    // --------------------------------
    // Getter
    // --------------------------------
    std::vector<const ServerObstacleData*>
        ServerObstacleManager::GetObstacles(int roundId, int turnId) const
    {
        int key = roundId * 100 + turnId;

        std::vector<const ServerObstacleData*> result;

        auto it = m_ObstacleData.find(key);
        if (it == m_ObstacleData.end())
            return result;

        for (const auto& o : it->second)
            result.push_back(&o);

        return result;
    }
}
