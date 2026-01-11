#include "pch.h"
#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::Create(MeshHandle mesh, MaterialHandle mat)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;
}

void Mesh::UpdateRenderItem(XMFLOAT4X4 mWorld)
{
    m_renderItem.mWorld = mWorld;
}
