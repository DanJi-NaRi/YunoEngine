#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

struct ServerObstacleData // 카드 정보
{
public:
    int roundId;
    int turnId;
    std::vector<int> tileIds;
    int obstacleType;
    int damage;
};

namespace yuno::server
{
    class ServerObstacleManager
    {
    public:
        bool LoadFromCSV(const std::string& path);

        std::vector<const ServerObstacleData*> GetObstacles(int roundId, int turnId) const;
    private:
        std::unordered_map<int, std::vector<ServerObstacleData>> m_ObstacleData;
    };
}
