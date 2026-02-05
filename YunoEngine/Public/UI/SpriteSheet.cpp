#include "pch.h"

#include "SpriteSheet.h"
#include <algorithm>

SpriteSheet::SpriteSheet(UIFactory& uiFactory) : Widget(uiFactory)
{
}

SpriteSheet::~SpriteSheet()
{
}

bool SpriteSheet::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);
    UpdateFrameConstants();
    return true;
}

bool SpriteSheet::Start()
{
    UpdateFrameConstants();
    return true;
}

bool SpriteSheet::Update(float dTime)
{
    Widget::Update(dTime);
    AdvanceAnimation(dTime);
    return true;
}

bool SpriteSheet::Submit(float dTime)
{
    UpdateFrameConstants();

    if (m_MeshNode)
    {
        for (auto& mesh : m_MeshNode->m_Meshs)
        {
            auto& renderItem = mesh->GetRenderItem();
            renderItem.isEffect = true;
            renderItem.effectConst.effectData = {
                static_cast<float>(m_frameIndex),
                m_elapsed,
                static_cast<float>(m_cols),
                static_cast<float>(m_rows)
            };
        }
    }

    Widget::Submit(dTime);
    return true;
}

void SpriteSheet::SetSpriteSheet(const std::wstring& texturePath, int cols, int rows, int frameCount, float fps, bool loop)
{
    m_spriteSheetPath = texturePath;
    SetGrid(cols, rows);
    SetFrameCount(frameCount);
    SetFPS(fps);
    SetLoop(loop);

    ApplySpriteMaterial();
}

void SpriteSheet::SetFrameIndex(int frameIndex)
{
    if (m_frameCount <= 0)
    {
        m_frameIndex = 0;
        return;
    }

    m_frameIndex = std::clamp(frameIndex, 0, m_frameCount - 1);
    UpdateFrameConstants();
}

void SpriteSheet::SetFrameCount(int frameCount)
{
    m_frameCount = std::max(1, frameCount);
    m_frameIndex = std::clamp(m_frameIndex, 0, m_frameCount - 1);
    UpdateFrameConstants();
}

void SpriteSheet::SetGrid(int cols, int rows)
{
    m_cols = std::max(1, cols);
    m_rows = std::max(1, rows);
    UpdateFrameConstants();
}

void SpriteSheet::SetFPS(float fps)
{
    m_fps = std::max(0.1f, fps);
}

void SpriteSheet::SetLoop(bool loop)
{
    m_loop = loop;
}

void SpriteSheet::Play()
{
    m_playing = true;
}

void SpriteSheet::Pause()
{
    m_playing = false;
}

void SpriteSheet::Stop(bool resetFrame)
{
    m_playing = false;
    m_frameTimer = 0.0f;
    m_elapsed = 0.0f;
    if (resetFrame)
        m_frameIndex = 0;
    UpdateFrameConstants();
}

bool SpriteSheet::CreateMaterial()
{
    return ApplySpriteMaterial();
}

bool SpriteSheet::ApplySpriteMaterial()
{
    if (!m_pTextures || !m_pRenderer)
        return false;

    m_Albedo = m_pTextures->LoadTexture2D(m_spriteSheetPath.c_str());

    MaterialDesc md{};
    md.passKey.vs = ShaderId::UISpriteSheet;
    md.passKey.ps = ShaderId::UISpriteSheet;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;
    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::Off;

    md.albedo = m_Albedo;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    m_defaultMaterial = m_pRenderer->CreateMaterial(md);
    if (m_defaultMaterial == 0)
        return false;

    if (m_MeshNode)
    {
        for (auto& mesh : m_MeshNode->m_Meshs)
            mesh->SetMaterial(m_defaultMaterial);
    }

    return true;
}

void SpriteSheet::AdvanceAnimation(float dTime)
{
    if (!m_playing || m_frameCount <= 1)
        return;

    m_elapsed += dTime;
    m_frameTimer += dTime;

    const float frameDuration = 1.0f / m_fps;
    while (m_frameTimer >= frameDuration)
    {
        m_frameTimer -= frameDuration;
        m_frameIndex++;

        if (m_frameIndex >= m_frameCount)
        {
            if (m_loop)
                m_frameIndex = 0;
            else
            {
                m_frameIndex = m_frameCount - 1;
                m_playing = false;
                break;
            }
        }
    }

    UpdateFrameConstants();
}

void SpriteSheet::UpdateFrameConstants()
{
    m_renderItem.Constant.widgetValueFloat = static_cast<float>(m_frameIndex);
    m_renderItem.Constant.widgetValueInt = PackGrid();
}

int SpriteSheet::PackGrid() const
{
    const int safeCols = std::clamp(m_cols, 0, 0xFFFF);
    const int safeRows = std::clamp(m_rows, 0, 0xFFFF);
    return (safeRows << 16) | safeCols;
}
