#pragma once

#include <asio.hpp>

class NetworkServer {
public:
    explicit NetworkServer(unsigned short port);

    void Run();

private:
    void StartAccept();

    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
};
