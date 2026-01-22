#include "pch.h"
#include "YunoTextureManager.h"

#include "YunoRenderer.h"

YunoTextureManager::YunoTextureManager(YunoRenderer* renderer)
    : m_renderer(renderer)
{
}


TextureHandle YunoTextureManager::LoadTexture2D(const wchar_t* path)
{
    if (!path || !path[0] || !m_renderer)
        return 0;

    const std::wstring key(path);

    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;


    const TextureHandle h = m_renderer->CreateTexture2DFromFile(path);

    if (h != 0)
        m_cache.emplace(key, h);

    return h;
}

void YunoTextureManager::Clear()
{
    m_cache.clear();
}

const std::pair<int,int> YunoTextureManager::GetTextureWH(TextureHandle handle) const
{
    if (!m_renderer)
        return { -1, -1 };
    const auto& textures = m_renderer->GetTextureSize(handle);

    return { static_cast<int>(textures.first), static_cast<int>(textures.second) };

}
