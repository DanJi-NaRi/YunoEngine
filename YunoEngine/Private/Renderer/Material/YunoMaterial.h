#pragma once

#include "RenderTypes.h"
#include "YunoConstantBuffers.h"

class YunoMaterial
{
public:
    RenderPassHandle pass = 0;

    CBMaterial cpuParams{};

    TextureHandle albedo = 0;
    TextureHandle normal = 0;
    TextureHandle orm = 0;
};
