#pragma once



namespace yuno::net::packets
{
    struct C2S_ReadySet final
    {
        std::uint8_t readyState = 0; // 0은 준비해제, 1은 준비완료

        void Serialize(ByteWriter& w) const;
        static C2S_ReadySet Deserialize(ByteReader& r);
    };
}
