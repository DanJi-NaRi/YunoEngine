#include "pch.h"
#include "ClientNet.h"

ClientNet::ClientNet()
    : socket_(io_)
    , resolver_(io_)
{
}

ClientNet::~ClientNet()
{
    Close();
}

bool ClientNet::Connect(const std::string& host, unsigned short port)
{
    try
    {
        auto endpoints = resolver_.resolve(host, std::to_string(port));
        boost::asio::connect(socket_, endpoints);

        connected_ = true;
        StartRead();
        return true;
    }
    catch (...)
    {
        connected_ = false;
        return false;
    }
}

void ClientNet::Pump()
{
    io_.poll();
}

void ClientNet::SendLine(const std::string& line)
{
    if (!connected_) return;

    std::string msg = line;
    msg.push_back('\n');

    EnqueueSend(std::move(msg));
}

void ClientNet::Close()
{
    if (!socket_.is_open())
        return;

    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);

    connected_ = false;
}

void ClientNet::StartRead()
{
    auto self = this;
    socket_.async_read_some(
        boost::asio::buffer(temp_),
        [this](const boost::system::error_code& error, std::size_t length)
        {
            if (error)
            {
                connected_ = false;
                return;
            }

            OnRead(length);
            StartRead();
        });
}

void ClientNet::OnRead(std::size_t length)
{
    inbuf_.append(temp_.data(), length);

    while (true)
    {
        const auto pos = inbuf_.find('\n');
        if (pos == std::string::npos)
            break;

        std::string line = inbuf_.substr(0, pos);
        inbuf_.erase(0, pos + 1);

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (onLine_)
            onLine_(line);
    }
}

void ClientNet::EnqueueSend(std::string msg)
{
    outq_.push_back(std::move(msg));
    if (!writing_)
        StartWrite();
}

void ClientNet::StartWrite()
{
    if (outq_.empty())
    {
        writing_ = false;
        return;
    }

    writing_ = true;

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(outq_.front()),
        [this](const boost::system::error_code& error, std::size_t /*length*/)
        {
            if (error)
            {
                connected_ = false;
                return;
            }

            outq_.pop_front();
            StartWrite();
        });
}
