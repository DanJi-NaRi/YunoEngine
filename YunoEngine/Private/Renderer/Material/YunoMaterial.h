#pragma once

#include "RenderTypes.h"
#include "YunoConstantBuffers.h"

class YunoMaterial
{
public:
    RenderPassHandle pass = 0;

    TextureHandle albedo = 0;
    TextureHandle normal = 0;

    TextureHandle orm = 0;
    TextureHandle metallic = 0;
    TextureHandle roughness = 0;
    TextureHandle ao = 0;

};
