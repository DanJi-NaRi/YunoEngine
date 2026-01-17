#include "NetworkServer.h"

#include "Session.h"

NetworkServer::NetworkServer(unsigned short port)
    : io_context_(),
    acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    StartAccept();
}

void NetworkServer::Run() {
    io_context_.run();
}

void NetworkServer::StartAccept() {
    acceptor_.async_accept(
        [this](const asio::error_code& error, asio::ip::tcp::socket socket) {
            if (!error) {
                std::make_shared<Session>(std::move(socket))->Start();
            }

            StartAccept();
        });
}
