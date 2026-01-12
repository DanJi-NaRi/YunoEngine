#pragma once
#include "RenderTypes.h"

struct MaterialDesc
{
    PassKey passKey;

    DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };
    float roughness = 0.5f;
    float metallic = 0.0f;

    TextureHandle albedo = 0;
    TextureHandle normal = 0;
    // 아트한테 채널별로 받을 수 있는지 체크하고 확정
    TextureHandle orm = 0;      // rough, metal, ao
    TextureHandle metal = 0;      // rough, metal, ao
    TextureHandle rough = 0;      // rough, metal, ao
    TextureHandle ao = 0;      // rough, metal, ao
};
