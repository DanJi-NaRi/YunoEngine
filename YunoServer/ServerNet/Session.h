#pragma once

#include <array>
#include <deque>
#include <memory>
#include <string>

#include <boost/asio.hpp>

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::ip::tcp::socket socket);

    void Start();

private:
    void DoRead();
    void OnRead(std::size_t length);

    void EnqueueSend(std::string msg);
    void DoWrite();

    void HandleLine(const std::string& line);

private:
    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_{};

    // ✅ 문자열 패킷용 버퍼/송신큐
    std::string inbuf_;                 // 수신 누적 버퍼
    std::deque<std::string> outq_;      // 송신 대기 큐
    bool writing_ = false;              // async_write 진행 중 여부
};
