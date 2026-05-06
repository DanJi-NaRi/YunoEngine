#pragma once

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>
namespace
{
    std::string ReadServerHostFromEnv()
    {
        char* buf = nullptr;
        size_t len = 0;

        if (_dupenv_s(&buf, &len, "YUNO_SERVER_HOST") != 0 || buf == nullptr)
        {
            std::cout << "[GameApp] YUNO_SERVER_HOST env not set. fallback=127.0.0.1\n";
            return "127.0.0.1";
        }

        std::string host(buf);
        free(buf);

        if (host.empty())
        {
            std::cout << "[GameApp] YUNO_SERVER_HOST is empty. fallback=127.0.0.1\n";
            return "127.0.0.1";
        }

        return host;
    }

    std::uint16_t ReadServerPortFromEnv()
    {
        char* buf = nullptr;
        size_t len = 0;

        if (_dupenv_s(&buf, &len, "YUNO_SERVER_PORT") != 0 || buf == nullptr)
        {
            std::cout << "[GameApp] YUNO_SERVER_PORT env not set. fallback=9000\n";
            return 9000;
        }

        std::uint16_t port = 9000;
        try
        {
            const unsigned long raw = std::stoul(buf);
            if (raw >= 1 && raw <= 65535)
                port = static_cast<std::uint16_t>(raw);
        }
        catch (...)
        {
            port = 9000;
        }

        free(buf);
        return port;
    }

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
