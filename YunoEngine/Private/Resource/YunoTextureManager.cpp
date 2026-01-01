#include "pch.h"
#include "YunoTextureManager.h"

#include "IRenderer.h"

YunoTextureManager::YunoTextureManager(IRenderer* renderer)
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

    // 렌더러가 실제 GPU 텍스처 생성 및 핸들 발급
    const TextureHandle h = m_renderer->CreateTexture2DFromFile(path);

    // 실패해도 캐시하지 말고(0은 “없음”이므로) 그대로 반환
    if (h != 0)
        m_cache.emplace(key, h);

    return h;
}

void YunoTextureManager::Clear()
{
    m_cache.clear();
}
