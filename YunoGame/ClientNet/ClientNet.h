#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <functional>
#include <string>

class ClientNet
{
public:
    using OnLineFn = std::function<void(const std::string&)>;

public:
    ClientNet();
    ~ClientNet();

    // 127.0.0.1, 9000 같은 형태
    bool Connect(const std::string& host, unsigned short port);

    void Pump();


    void SendLine(const std::string& line);

    void Close();

    bool IsConnected() const { return connected_; }

    void SetOnLine(OnLineFn fn) { onLine_ = std::move(fn); }

private:
    void StartRead();
    void OnRead(std::size_t length);

    void EnqueueSend(std::string msg);
    void StartWrite();

private:
    boost::asio::io_context io_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;

    std::array<char, 1024> temp_{};

    std::string inbuf_;
    std::deque<std::string> outq_;
    bool writing_ = false;
    bool connected_ = false;

    OnLineFn onLine_;
};
