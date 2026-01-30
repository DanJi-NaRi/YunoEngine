#include <iostream>
#include "ServerCardRuntime.h"

using namespace yuno::server;

uint32_t ServerCardRuntime::CreateCard(uint32_t dataID)
{
    uint32_t runtimeID = ++m_nextRuntimeID;
    m_runtimeToData[runtimeID] = dataID;

    std::cout
        << "[Server] CreateCard runtimeID="
        << runtimeID
        << " dataID="
        << dataID
        << std::endl;

    return runtimeID;
}
