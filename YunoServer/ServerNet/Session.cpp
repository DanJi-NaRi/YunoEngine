#include "Session.h"

Session::Session(asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

void Session::Start() {
    DoRead();
}

void Session::DoRead() {
    auto self = shared_from_this();
    socket_.async_read_some(
        asio::buffer(buffer_),
        [self](const asio::error_code& error, std::size_t length) {
            if (error) {
                return;
            }

            self->DoWrite(length);
        });
}

void Session::DoWrite(std::size_t length) {
    auto self = shared_from_this();
    asio::async_write(
        socket_,
        asio::buffer(buffer_.data(), length),
        [self](const asio::error_code& error, std::size_t /*length*/) {
            if (error) {
                return;
            }

            self->DoRead();
        });
}
