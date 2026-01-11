#include "pch.h"
#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::Create(MeshHandle mesh, MaterialHandle mat, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_vPos = pos;
    m_vRot = rot;
    m_vScale = scale;

    XMMATRIX userTM = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z)
                                        * XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z)
                                        * XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMStoreFloat4x4(&m_mUser, userTM);

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;
}

void Mesh::UpdateRenderItem(XMFLOAT4X4 mWorld)
{
    m_renderItem.mWorld = mWorld;
}
