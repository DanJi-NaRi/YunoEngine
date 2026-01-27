#pragma once
#include "RenderTypes.h"

class ITextureManager
{
public:
    virtual ~ITextureManager() = default;

    // 경로 기반 2D 텍스처 로드(캐시 포함)
    virtual TextureHandle LoadTexture2D(const wchar_t* path) = 0;

    virtual const std::pair<int, int> GetTextureWH(TextureHandle handle) = 0;
    // (선택) 캐시 비우기 / 언로드 정책은 나중에 확장
    virtual void Clear() = 0;
};
