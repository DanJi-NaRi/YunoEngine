#include "Session.h"

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

void Session::Start() {
    DoRead();
}

void Session::DoRead() {
    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer(buffer_),
        [self](const boost::system::error_code& error, std::size_t length) {
            if (error) {
                return;
            }

            self->DoWrite(length);
        });
}

void Session::DoWrite(std::size_t length) {
    auto self = shared_from_this();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(buffer_.data(), length),
        [self](const boost::system::error_code& error, std::size_t /*length*/) {
            if (error) {
                return;
            }

            self->DoRead();
        });
}
