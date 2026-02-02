#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <stdexcept>

namespace yuno::net
{
    // =========================
    // 8 / 16 / 32 비트 LE R/W 함수들
    // =========================
    std::uint8_t  ReadU8    (const std::uint8_t* p);
    std::uint16_t ReadU16LE (const std::uint8_t* p);
    std::uint32_t ReadU32LE (const std::uint8_t* p);

    void WriteU8(std::uint8_t* p, std::uint8_t v);
    void WriteU16LE(std::uint8_t* p, std::uint16_t v);
    void WriteU32LE(std::uint8_t* p, std::uint32_t v);

    // =========================
    // Pointer 기반 바이트 Reader
    // =========================
    class ByteReader
    {
    public:
        ByteReader(const std::uint8_t* data, std::size_t size);

        bool Has(std::size_t bytes) const;

        std::uint8_t  ReadU8();
        std::uint16_t ReadU16LE();
        std::uint32_t ReadU32LE();

        const std::uint8_t* Current() const { return m_cur; }
        std::size_t Remaining() const { return m_end - m_cur; }

    private:
        const std::uint8_t* m_cur;
        const std::uint8_t* m_end;
    };

    // =========================
    // Pointer 기반 바이트 Writer
    // =========================
    class ByteWriter
    {
    public:
        explicit ByteWriter(std::vector<std::uint8_t>& out);

        void WriteU8(std::uint8_t v);
        void WriteU16LE(std::uint16_t v);
        void WriteU32LE(std::uint32_t v);

    private:
        std::vector<std::uint8_t>& m_out;
    };
}
