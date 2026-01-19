#pragma once

#include <array>
#include <memory>

#include <boost/asio.hpp>

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::ip::tcp::socket socket);

    void Start();

private:
    void DoRead();
    void DoWrite(std::size_t length);

    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_{};
};
