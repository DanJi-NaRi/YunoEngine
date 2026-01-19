#pragma once

#include <boost/asio.hpp>

class NetworkServer {
public:
    explicit NetworkServer(unsigned short port);
    void Run();

private:
    void StartAccept();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
};
