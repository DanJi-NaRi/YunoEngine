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
    TextureHandle orm = 0;      // rough, metal, ao
    TextureHandle metal = 0;      // rough, metal, ao
    TextureHandle rough = 0;      // rough, metal, ao
    TextureHandle ao = 0;      // rough, metal, ao
};
