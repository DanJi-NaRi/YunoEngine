#pragma once

#include "RenderTypes.h"
#include "YunoConstantBuffers.h"

class YunoMaterial
{
public:
    RenderPassHandle pass = 0;

    DirectX::XMFLOAT4 baseColor = { 1, 1, 1, 1 };
    float roughRatio = 1.0f;
    float metalRatio = 1.0f;

    TextureHandle albedo = 0;
    TextureHandle normal = 0;
    TextureHandle orm = 0;
};
