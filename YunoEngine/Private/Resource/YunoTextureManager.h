#pragma once

#include "ITextureManager.h"


class IRenderer;

class YunoTextureManager final : public ITextureManager
{
public:
    explicit YunoTextureManager(IRenderer* renderer);

    TextureHandle LoadTexture2D(const wchar_t* path) override;
    void Clear() override;

private:
    IRenderer* m_renderer = nullptr;
    std::unordered_map<std::wstring, TextureHandle> m_cache;
};
