#include "pch.h"

#include "Mesh.h"
#include "IRenderer.h"
#include "YunoEngine.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::Create(MeshHandle mesh, MaterialHandle mat, const XMMATRIX& modelTM)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_mUser = modelTM;

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;
}

void Mesh::Create(MeshHandle mesh, MaterialHandle mat, const XMFLOAT3& vPos, const XMFLOAT3& vRot, const XMFLOAT3& vScale)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_mUser = XMMatrixScaling(vScale.x, vScale.y, vScale.z) 
                                        * XMMatrixRotationRollPitchYaw(vRot.x, vRot.y, vRot.z) 
                                        * XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;
}

void Mesh::UpdateRenderItem(XMFLOAT4X4 mWorld)
{
    m_renderItem.mWorld = mWorld;
}

void Mesh::Submit(XMFLOAT4X4& mWorld)
{
    XMStoreFloat4x4(&m_renderItem.mWorld, m_mUser * XMLoadFloat4x4(&mWorld));
}

void Mesh::LastSubmit()
{
    YunoEngine::GetRenderer()->Submit(m_renderItem);
}

void MeshNode::Submit(XMFLOAT4X4& mWorld)
{
    for (auto& mesh : m_Meshs)
        mesh->Submit(mWorld);

    for (auto& child : m_Childs)
        child->Submit(mWorld);
}

void MeshNode::LastSubmit()
{
    for (auto& mesh : m_Meshs)
        mesh->LastSubmit();

    for (auto& child : m_Childs)
        child->LastSubmit();
}
