#include "pch.h"

#include "EffectManager.h"
#include "IRenderer.h"

EffectManager::EffectManager()
{
    m_pRenderer = YunoEngine::GetRenderer();
}

EffectManager::~EffectManager()
{
    m_pool.clear();
}

void EffectManager::Init(int count)
{
    for (int i = 0; i < count; i++)
    {
        auto e = std::make_unique<Effect>();
        m_free.push_back(e.get());
        m_pool.push_back(std::move(e));
    }
}

bool EffectManager::RegisterEffect(EffectID id, ShaderId shaderid, BillboardMode mode, float lifetime, int framecount, int cols, int rows, const std::wstring& texturePath)
{
    auto it = m_templates.find(id);

    if (it == m_templates.end())
        return true;

    EffectTemplate temp{};

    temp.mesh = m_pRenderer->GetQuadMesh();

    TextureHandle h = m_pRenderer->CreateColorTexture2DFromFile(texturePath.c_str());

    if (!h) return false;
    temp.texture = h;

    MaterialDesc md{};

    md.passKey.vs = shaderid;
    md.passKey.ps = shaderid;
    md.passKey.blend = BlendPreset::ColorBlendOne;
    md.passKey.depth = DepthPreset::ReadOnly;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;
    md.albedo = h;

    MaterialHandle mh = m_pRenderer->CreateMaterial(md);

    if (!mh) return false;

    temp.material = mh;
    temp.lifetime = lifetime;
    temp.frameCount = framecount;
    temp.cols = cols;
    temp.rows = rows;
    temp.billboard = mode;

    m_templates[id] = temp;
    return true;
}

Effect* EffectManager::Spawn(EffectID id, const XMFLOAT3& pos, const XMFLOAT3& scale, const XMFLOAT3& dir)
{
    auto it = m_templates.find(id);
    if (it == m_templates.end())
        return nullptr;

    if (m_free.empty()) return nullptr;

    Effect* e = m_free.back();
    m_free.pop_back();

    e->SetTemplate(it->second);
    e->Play(pos, scale, dir);
    m_active.push_back(e);

    return e;
}

void EffectManager::Update(float dt)
{
    for (auto it = m_active.begin(); it != m_active.end(); )
    {
        Effect* e = *it;

        if (!e->Update(dt))
        {
            e->Reset();
            m_free.push_back(e);
            it = m_active.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void EffectManager::Submit(float dt)
{
    for (auto* e : m_active)
        e->Submit(dt);
}
