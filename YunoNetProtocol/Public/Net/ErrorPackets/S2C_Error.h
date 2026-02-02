#pragma once

#include "PacketType.h"

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{

    struct S2C_Error final
    {
        // 에러코드
        ErrorCode code = ErrorCode::None;
        EnterDeniedReason  reason = EnterDeniedReason::None;

        // 어떤 요청에 대해서 애러가 발생했는지
        PacketType contextType = PacketType::Invalid;

        void Serialize(ByteWriter& w) const;
        static S2C_Error Deserialize(ByteReader& r);
    };
}
