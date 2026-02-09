#include <iostream>
#include "ServerCardRuntime.h"

using namespace yuno::server;

uint32_t ServerCardRuntime::CreateCard(uint32_t dataID)
{
    uint32_t runtimeID = ++m_nextRuntimeID;
    m_runtimeToData[runtimeID] = dataID;

    //std::cout
    //    << "[Server] CreateCard runtimeID="
    //    << runtimeID
    //    << " dataID="
    //    << dataID
    //    << std::endl;

    return runtimeID;
}

void ServerCardRuntime::RemoveCard(uint32_t runtimeID)
{
    auto it = m_runtimeToData.find(runtimeID);
    if (it == m_runtimeToData.end())
    {
        std::cout
            << "[Server] RemoveCard FAILED runtimeID="
            << runtimeID
            << " (not found)\n";
        return;
    }

    std::cout
        << "[Server] RemoveCard runtimeID="
        << runtimeID
        << " dataID="
        << it->second
        << std::endl;

    m_runtimeToData.erase(it);
}
