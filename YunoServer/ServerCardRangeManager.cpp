#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "ServerCardRangeManager.h"

namespace yuno::server
{
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

    bool ServerCardRangeManager::LoadFromCSV(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::string line;
        std::getline(file, line); // header skip

        while (std::getline(file, line))
        {
            auto cols = Split(line, ',');
            if (cols.empty()) continue;

            RemoveBOM(cols[0]);

            RangeData range;
            range.rangeId = std::stoul(cols[0]);

            for (size_t i = 1; i < cols.size(); ++i)
            {
                if (cols[i] == "0" || cols[i].empty())
                    continue;

                auto pos = cols[i].find(';');
                if (pos == std::string::npos)
                    continue;

                int dx = std::stoi(cols[i].substr(0, pos));
                int dy = std::stoi(cols[i].substr(pos + 1));

                range.offsets.push_back({ dx, dy });
            }

            m_ranges[range.rangeId] = range;
        }

        std::cout << "[ServerCardRangeManager] Loaded Ranges: "
            << m_ranges.size() << "\n";

        return true;
    }

    const RangeData* ServerCardRangeManager::GetRange(uint32_t rangeId) const
    {
        auto it = m_ranges.find(rangeId);
        if (it == m_ranges.end())
            return nullptr;
        return &it->second;
    }

}
