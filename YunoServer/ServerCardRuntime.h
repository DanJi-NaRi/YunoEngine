#pragma once
#include <unordered_map>
#include <cstdint>

namespace yuno::server
{
    class ServerCardRuntime
    {
    public:
        uint32_t CreateCard(uint32_t dataID);

        uint32_t GetDataID(uint32_t runtimeID) const
        {
            return m_runtimeToData.at(runtimeID);
        }

    private:
        uint32_t m_nextRuntimeID = 0;
        std::unordered_map<uint32_t, uint32_t> m_runtimeToData;
    };
}

