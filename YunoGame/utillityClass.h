#pragma once

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>
namespace
{
    std::uint32_t ReadUserIdFromEnv()
    {
        char* buf = nullptr;
        size_t len = 0;

        if (_dupenv_s(&buf, &len, "YUNO_USER_ID") != 0 || buf == nullptr)
        {
            std::cout << "[GameApp] YUNO_USER_ID env not set\n";
            return 0;
        }

        std::uint32_t uid = 0;
        try
        {
            uid = static_cast<std::uint32_t>(std::stoul(buf));
        }
        catch (...)
        {
            uid = 0;
        }

        free(buf);
        return uid;
    }
}
