#include "pch.h"

#include "EffectUnit.h"

#include "ObjectManager.h"
#include "ObjectTypeRegistry.h"
#include "UImgui.h"

namespace {
    struct AutoReg_EffectUnit
    {
        AutoReg_EffectUnit()
        {
            ObjectTypeRegistry::Instance().Register(L"EffectUnit", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<EffectUnit>(d); });
        }
    } s_reg_effectUnit;
}

EffectUnit::EffectUnit()
{
    unitType = L"EffectUnit";
}

bool EffectUnit::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    if (!Unit::Create(name, id, vPos))
    {
        return false;
    }
    m_active = false;

    return true;
}

bool EffectUnit::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, PassOption opt)
{
    (void)opt;

    if (!Unit::Create(name, id, vPos))
    {
        return false;
    }

    m_active = false;

    return true;
}

bool EffectUnit::BuildInternalEffectMaterial(const EffectDesc& desc)
{
    if (!m_pRenderer)
    {
        return false;
    }

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
    md.passKey.depth = DepthPreset::ReadWrite;
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
    temp.isLoop = desc.isLoop;

    SetTemplate(temp);

    m_active = m_renderItem.meshHandle != 0 && m_renderItem.materialHandle != 0;

    return m_active;
}


bool EffectUnit::Update(float dt)
{
    if (!m_active)
    {
        return true;
    }

    if (!Effect::Update(dt))
    {
        Reset();

        if (GetParent())
        {
            DettachParent();
        }
    }

    return true;
}

void EffectUnit::Stop()
{
    Reset();

    if (GetParent())
    {
        DettachParent();
    }
}

#ifdef _DEBUG
void EffectUnit::Serialize()
{
    Unit::Serialize();

    if (UI::CollapsingHeader("Emissive"))
    {
        UI::DragFloat3Editable("Color", &m_emissiveCol.x, 0.001f, 0.0f, 1.0f);
        UI::DragFloatEditable("Color", &m_emissive, 0.01f, 0.0f, 50.0f);
    }
}
#endif
