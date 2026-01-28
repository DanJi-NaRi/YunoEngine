#pragma once

#include "ITextureManager.h"


class YunoRenderer;
class TextureResource;

class YunoTextureManager final : public ITextureManager
{
public:
    explicit YunoTextureManager(YunoRenderer* renderer);

    TextureHandle LoadTexture2D(const wchar_t* path) override;
    void Clear() override;

    const std::pair<int,int> GetTextureWH(TextureHandle handle) override;

private:
    YunoRenderer* m_renderer = nullptr;
    std::unordered_map<std::wstring, TextureHandle> m_cache;
};



