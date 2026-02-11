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
    m_active = Enable;
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

void EffectUnit::UpdateWorldMatrix()
{
    XMMATRIX S = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX T = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX R1 = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    XMMATRIX R2 = UpdateBillBoard();
    XMMATRIX R = R1 * R2;

    XMMATRIX mTM;

    if (m_Parent)
    {
        XMMATRIX3 scaleTMs = GetScaleTMs(m_Parent->GetScale());
        XMMATRIX3 rotTMs = GetRotTMs(m_Parent->GetRot());

        XMMATRIX pScaleTM = GetIgnoreScaleTM(scaleTMs);
        XMMATRIX pRotTM = GetIgnoreRotTM(rotTMs);

        auto pPos = m_Parent->GetPos();
        XMMATRIX pTransTM = XMMatrixTranslation(pPos.x, pPos.y, pPos.z);

        mTM = S * R * T * pScaleTM * pRotTM * pTransTM;
    }
    else
        mTM = S * R * T;

    XMStoreFloat4x4(&m_mWorld, mTM);
}

void EffectUnit::Stop()
{
    Reset();

    if (GetParent())
    {
        DettachParent();
    }
}

void EffectUnit::IgnoreRotation(bool ignoreX, bool ignoreY, bool ignoreZ)
{
    m_rotIgnore = { ignoreX, ignoreY, ignoreZ };
}

void EffectUnit::IgnoreScale(bool ignoreX, bool ignoreY, bool ignoreZ)
{
    m_scaleIgnore = { ignoreX, ignoreY, ignoreZ };
}

XMMATRIX3 EffectUnit::GetRotTMs(XMFLOAT3 rot)
{
    XMMATRIX3 res;
    res.x = XMMatrixRotationX(rot.x);
    res.y = XMMatrixRotationX(rot.y);
    res.z = XMMatrixRotationX(rot.z);
    return res;
}

XMMATRIX3 EffectUnit::GetScaleTMs(XMFLOAT3 scale)
{
    XMMATRIX3 res;
    res.x = XMMatrixScaling(scale.x, 1, 1);
    res.y = XMMatrixScaling(1, scale.y, 1);
    res.z = XMMatrixScaling(1, 1, scale.z);
    return res;
}

XMMATRIX EffectUnit::GetIgnoreRotTM(XMMATRIX3 rotTMs)
{
    XMMATRIX res;
    XMMATRIX identity = XMMatrixIdentity();
    res = ((m_rotIgnore.x) ? identity : rotTMs.x) *
        ((m_rotIgnore.y) ? identity : rotTMs.y) *
        ((m_rotIgnore.z) ? identity : rotTMs.z);

    return res;
}

XMMATRIX EffectUnit::GetIgnoreScaleTM(XMMATRIX3 scaleTMs)
{
    XMMATRIX res;
    XMMATRIX identity = XMMatrixIdentity();
    res = ((m_rotIgnore.x) ? identity : scaleTMs.x) *
        ((m_rotIgnore.y) ? identity : scaleTMs.y) *
        ((m_rotIgnore.z) ? identity : scaleTMs.z);

    return res;
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
