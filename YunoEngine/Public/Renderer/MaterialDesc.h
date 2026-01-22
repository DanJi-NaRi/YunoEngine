#pragma once
#include "RenderTypes.h"



struct MaterialDesc
{
    PassKey passKey;

    TextureHandle albedo = 0;
    TextureHandle normal = 0;
    // 아트한테 채널별로 받을 수 있는지 체크하고 확정
    TextureHandle orm = 0;      // rough, metal, ao
    TextureHandle emissive = 0;

    TextureHandle metal = 0;    
    TextureHandle rough = 0;    
    TextureHandle ao = 0;    
};
