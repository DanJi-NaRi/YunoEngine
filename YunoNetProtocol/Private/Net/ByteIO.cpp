#include "pch.h"
#include "ByteIO.h"

namespace yuno::net
{
    // =========================
    // 8 / 16 / 32 비트 LE R/W 함수들
    // =========================
    std::uint8_t ReadU8(const std::uint8_t* p)
    {
        return p[0];
    }

    std::uint16_t ReadU16LE(const std::uint8_t* p)
    {
        return static_cast<std::uint16_t>(p[0])
            | (static_cast<std::uint16_t>(p[1]) << 8);
    }

    std::uint32_t ReadU32LE(const std::uint8_t* p)
    {
        return static_cast<std::uint32_t>(p[0])
            | (static_cast<std::uint32_t>(p[1]) << 8)
            | (static_cast<std::uint32_t>(p[2]) << 16)
            | (static_cast<std::uint32_t>(p[3]) << 24);
    }

    void WriteU8(std::uint8_t* p, std::uint8_t v)
    {
        p[0] = v;
    }

    void WriteU16LE(std::uint8_t* p, std::uint16_t v)
    {
        p[0] = static_cast<std::uint8_t>(v & 0xFFu);
        p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFFu);
    }

    void WriteU32LE(std::uint8_t* p, std::uint32_t v)
    {
        p[0] = static_cast<std::uint8_t>(v & 0xFFu);
        p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFFu);
        p[2] = static_cast<std::uint8_t>((v >> 16) & 0xFFu);
        p[3] = static_cast<std::uint8_t>((v >> 24) & 0xFFu);
    }

    // =========================
    // Pointer 기반 바이트 Reader
    // =========================
    ByteReader::ByteReader(const std::uint8_t* data, std::size_t size)
        : m_cur(data), m_end(data + size)
    {
    }

    bool ByteReader::Has(std::size_t bytes) const
    {
        return static_cast<std::size_t>(m_end - m_cur) >= bytes;
    }

    std::uint8_t ByteReader::ReadU8()
    {
        if (!Has(1)) throw std::runtime_error("ByteReader overflow (U8)");
        return *m_cur++;
    }

    std::uint16_t ByteReader::ReadU16LE()
    {
        if (!Has(2)) throw std::runtime_error("ByteReader overflow (U16)");
        auto v = yuno::net::ReadU16LE(m_cur);
        m_cur += 2;
        return v;
    }

    std::uint32_t ByteReader::ReadU32LE()
    {
        if (!Has(4)) throw std::runtime_error("ByteReader overflow (U32)");
        auto v = yuno::net::ReadU32LE(m_cur);
        m_cur += 4;
        return v;
    }

    // =========================
    // Pointer 기반 바이트 Writer
    // =========================
    ByteWriter::ByteWriter(std::vector<std::uint8_t>& out)
        : m_out(out)
    {
    }

    void ByteWriter::WriteU8(std::uint8_t v)
    {
        m_out.push_back(v);
    }

    void ByteWriter::WriteU16LE(std::uint16_t v)
    {
        m_out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
        m_out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
    }

    void ByteWriter::WriteU32LE(std::uint32_t v)
    {
        m_out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
        m_out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
        m_out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFFu));
        m_out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFFu));
    }
}
