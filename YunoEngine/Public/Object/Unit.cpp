#include "pch.h"

#include "Unit.h"
#include "Mesh.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Unit
    {
        AutoReg_Unit()
        {
            ObjectTypeRegistry::Instance().Register(L"Unit", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Unit>(d); });
        }
    } s_reg_unit;
}


Unit::Unit()
{
    m_vPos = XMFLOAT3(0, 0, 0);
    m_vRot = XMFLOAT3(0, 0, 0);
    m_vScale = XMFLOAT3(1, 1, 1);
    XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mRot, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mScale, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mTrans, XMMatrixIdentity());

    m_vDir = XMFLOAT3(0, 0, 1);

    m_Hp = 0;
    m_Mana = 0;
    m_Lv = 0;
    m_Exp = 0;

    m_time = 0.0f;

    m_defaultMesh = 0;
    m_defaultMaterial = 0;
    m_Albedo = 0;
    m_Normal = 0;
    m_Orm = 0;

    m_Parent = nullptr;
    unitType = L"Unit";
    m_meshpath.clear();
}

Unit::~Unit()
{

}

bool Unit::Create(XMFLOAT3 vPos)
{
    m_vPos = vPos;
    m_vRot = XMFLOAT3(0, 0, 0);
    m_vScale = XMFLOAT3(1, 1, 1);

    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    Backup();

    Unit::Update();

    return true;
}

bool Unit::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    m_id = id;
    m_name = name;

    Create(vPos);

    return true;
}

bool Unit::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale)
{
    m_id = id;
    m_name = name;

    m_vPos = vPos;
    m_vRot = vRot;
    m_vScale = vScale;

    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    Backup();

    Unit::Update();

    return true;
}

bool Unit::Update(float dTime)
{
    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM;

    if(m_Parent)
        mTM = mScale * mRot * mTrans * m_Parent->GetWorldMatrix();
    else
        mTM = mScale * mRot * mTrans;

    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    return true;
}

bool Unit::Submit(float dTime)
{
    if (!m_MeshNode) return true;
    m_MeshNode->Submit(m_mWorld, m_vPos);

    LastSubmit(dTime);

    return true;
}

bool Unit::LastSubmit(float dTime /*= 0*/)
{
    m_MeshNode->LastSubmit();
    
    return true;
}

void Unit::Backup()
{
    m_vPosBk = m_vPos;
    m_vRotBk = m_vRot;
    m_vScaleBk = m_vScale;
    m_vDirBk = m_vDir;
}

void Unit::SetMesh(std::unique_ptr<MeshNode>&& mesh)
{
    m_MeshNode = std::move(mesh);

    SaveMesh(m_MeshNode, m_Meshs);
}

void Unit::SaveMesh(std::unique_ptr<MeshNode>& node, std::vector<Mesh*>& out)
{
    for (auto& mesh : node->m_Meshs)
        out.push_back(mesh.get());

    for (auto& child : node->m_Childs)
        SaveMesh(child, out);
}

void Unit::SetEmissive(int num, const XMFLOAT4& color, float pow)
{
    if (num >= m_Meshs.size() && num < 0)
        return;

    m_Meshs[num]->SetEmissiveColor(color);
    m_Meshs[num]->SetEmissivePow(pow);
}

void Unit::SetEmissiveColor(int num, const XMFLOAT4& color)
{
    if (num >= m_Meshs.size() && num < 0)
        return;

    m_Meshs[num]->SetEmissiveColor(color);
}

void Unit::SetEmissivePow(int num, float pow)
{
    if (num >= m_Meshs.size() && num < 0)
        return;

    m_Meshs[num]->SetEmissivePow(pow);
}

void Unit::SetTexture(UINT meshindex, TextureUse use, const std::wstring& filepath)
{
    if (meshindex >= m_Meshs.size())
        return;

    m_Meshs[meshindex]->SetTexture(use, filepath);
}

void Unit::SetMaskColor(const XMFLOAT4& col)
{
    for (auto& m : m_Meshs)
    {
        m->SetMaskColor(col);
    }
}

void Unit::SetOpacity(const float opacity)
{
    for (auto& m : m_Meshs)
    {
        m->SetOpacity(opacity);
    }
}


void Unit::Attach(Unit* obj) //this가 부모, 파라미터로 자식
{
    if (obj->m_Parent)//기존 부모있으면 떨어진 후 결합
        obj->DettachParent();

    obj->m_Parent = this;

    m_Childs.insert(std::make_pair(obj->GetID(), obj));
}

void Unit::DettachParent()
{
    m_Parent->DettachChild(m_id);
}

void Unit::DettachChild(uint32_t id)
{
    if (m_Childs.find(id) == m_Childs.end())
        return;

    m_Childs.erase(id);
}

void Unit::ClearChild()
{
    if (m_Childs.empty())
        return;

    for (auto& [id, child] : m_Childs)
    {
        child->DettachParent();
    }

    m_Childs.clear();
}

XMMATRIX Unit::GetAttachMatrixForChild(Unit* child)
{
    return GetWorldMatrix();
}

UnitDesc Unit::GetDesc()
{
    UnitDesc d;
    d.ID = m_id;
    if (m_Parent)
        d.parentID = m_Parent->GetID();
    else
        d.parentID = 0;

    d.name = m_name;
    d.meshPath = m_meshpath;
    d.unitType = unitType;

    d.transform.position = FromXM(m_vPos);
    Vec3Desc degRot = { XMConvertToDegrees(m_vRot.x),  XMConvertToDegrees(m_vRot.y),  XMConvertToDegrees(m_vRot.z) };
    d.transform.rotation = degRot; // deg
    d.transform.scale = FromXM(m_vScale);

    std::vector<MeshDesc> mds;
    int num = 0;
    for (auto& m : m_Meshs)
    {
        MeshDesc md = m->BuildDesc();
        md.meshNum = num;

        d.MatDesc.push_back(md);
        num++;
    }

    return d;
}

#ifdef _DEBUG
void Unit::Serialize()
{
    int i = 0;
    for (auto& mesh : m_Meshs)
    {
        mesh->Serialize(i);
        i++;
    }
}
#endif
