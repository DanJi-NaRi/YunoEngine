#include "Session.h"

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {
}

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
            self->OnRead(length);
            self->DoRead();
        });
}

void Session::OnRead(std::size_t length) {
    // 1) 받은 바이트를 누적 버퍼에 추가
    inbuf_.append(buffer_.data(), length);

    while (true) {
        const auto pos = inbuf_.find('\n');
        if (pos == std::string::npos)
            break;

        std::string line = inbuf_.substr(0, pos);
        inbuf_.erase(0, pos + 1);

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        HandleLine(line);
    }
}

void Session::HandleLine(const std::string& line) {

    if (line == "C2S") {
        EnqueueSend("S2C\n");
        return;
    }

    EnqueueSend("ECHO " + line + "\n");
}

void Session::EnqueueSend(std::string msg) {
    outq_.push_back(std::move(msg));
    if (!writing_) {
        DoWrite();
    }
}

void Session::DoWrite() {
    if (outq_.empty()) {
        writing_ = false;
        return;
    }

    writing_ = true;
    auto self = shared_from_this();

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(outq_.front()),
        [self](const boost::system::error_code& error, std::size_t /*length*/) {
            if (error) {
                return;
            }

            self->outq_.pop_front();
            self->DoWrite();
        });
}
