#include "pch.h"

#include "Unit.h"
#include "Mesh.h"


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

    if (!m_Meshs.empty())
    {
        XMMATRIX userTM = XMLoadFloat4x4(&m_Meshs[0]->GetUserTM());
        mTM = userTM * mTM;
    }
        
    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);


    return true;
}

bool Unit::Submit(float dTime)
{
    for (auto& mesh : m_Meshs)
    {
        mesh->SetObjectConstants(m_constant);
        mesh->UpdateRenderItem(m_mWorld);
    }

    LastSubmit(dTime);

    return true;
}

bool Unit::LastSubmit(float dTime /*= 0*/)
{
    for (auto& mesh : m_Meshs)
    {
        YunoEngine::GetRenderer()->Submit(mesh->GetRenderItem());
    }
    
    return true;
}

void Unit::Backup()
{
    m_vPosBk = m_vPos;
    m_vRotBk = m_vRot;
    m_vScaleBk = m_vScale;
    m_vDirBk = m_vDir;
}

void Unit::SetMesh(std::unique_ptr<Mesh>&& mesh)
{
    m_Meshs.push_back(std::move(mesh));
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

