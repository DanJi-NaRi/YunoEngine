#include "pch.h"

#include "EffectManager.h"
#include "IRenderer.h"
#include "UImgui.h"

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

bool EffectManager::RegisterEffect(const EffectDesc& desc)
{
    auto it = m_templates.find(desc.id);

    if (it != m_templates.end())
        return true;

    EffectTemplate temp{};

    temp.mesh = m_pRenderer->GetQuadMesh();

    TextureHandle h = m_pRenderer->CreateColorTexture2DFromFile(desc.texPath.c_str());

    if (!h) return false;
    temp.texture = h;

    MaterialDesc md{};

    md.passKey.vs = desc.shaderid;
    md.passKey.ps = desc.shaderid;
    md.passKey.blend = BlendPreset::AlphaBlend;
    //md.passKey.blend = BlendPreset::ColorBlendOne;
    md.passKey.depth = DepthPreset::ReadOnly;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;
    md.albedo = h;

    MaterialHandle mh = m_pRenderer->CreateMaterial(md);

    if (!mh) return false;

    temp.material = mh;
    temp.lifetime = desc.lifetime;
    temp.emissive = desc.emissive;
    temp.color = desc.color;
    temp.rot = desc.rot;
    temp.frameCount = desc.framecount;
    temp.cols = desc.cols;
    temp.rows = desc.rows;
    temp.billboard = desc.billboard;

    m_templates[desc.id] = temp;
    return true;
}

void EffectManager::SetEmissive(EffectID id, float emissive)
{
    auto it = m_templates.find(id);
    if (it == m_templates.end())
        return;

    it->second.emissive = emissive;
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

void EffectManager::Play()
{
    isPlay = true;
}

void EffectManager::Stop()
{
    isPlay = false;
}

void EffectManager::Update(float dt)
{
    for (auto it = m_active.begin(); it != m_active.end(); )
    {
        Effect* e = *it;

        float dTime;
        if (isPlay)
            dTime = dt;
        else
            dTime = 0;

        if (!e->Update(dTime))
        {
            e->Reset();
            m_free.push_back(e);
            it = m_active.erase(it);

            if (e->GetParent())
            {
                e->DettachParent();
            }
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

#ifdef _DEBUG
void EffectManager::Serialize()
{
    UI::BeginPanel("Effect Templates");

    if (m_templates.empty())
    {
        UI::Text("No Effect Templates Registered.");
        UI::EndPanel();
        return;
    }

    // 왼쪽 리스트
    UI::Text("Templates:");
    UI::Separator();

    for (auto& [id, temp] : m_templates)
    {
        bool selected = (id == m_selectedTemplate);

        std::string label = "Effect_" + std::to_string((int)id);

        if (UI::Selectable(label.c_str(), selected))
        {
            m_selectedTemplate = id;
        }
    }

    UI::EndPanel();


    // ===========================
    // 선택된 Template Inspector
    // ===========================
    if (m_selectedTemplate != EffectID::Count)
    {
        auto it = m_templates.find(m_selectedTemplate);
        if (it == m_templates.end())
            return;

        EffectTemplate& temp = it->second;

        UI::BeginPanel("Effect Inspector");

        UI::Text("Selected Template ID: %d", m_selectedTemplate);
        UI::Separator();

        // Emissive만 조절 가능
        float emissive = temp.emissive;
        XMFLOAT4 color = temp.color;

        if (UI::DragFloatEditable("Emissive", &emissive, 0.01f, 0.0f, 100.0f))
        {
            temp.emissive = emissive;
        }
        if (UI::DragFloat3Editable("EmissiveColor", &color.x, 0.01f, 0.0f, 1.f))
        {
            temp.color = color;
        }

        UI::EndPanel();
    }
}
#endif
