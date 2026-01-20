#include "pch.h"

#include "Mesh.h"
#include "IRenderer.h"
#include "YunoEngine.h"
#include "YunoRenderer.h"

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

void Mesh::Create(MeshHandle mesh, MaterialHandle mat, const XMFLOAT3& vPos, const XMFLOAT3& vRot, const XMFLOAT3& vScale)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;
}

void Mesh::SetTexture(TextureUse use, const std::wstring& filepath)
{
    const auto& renderer = YunoEngine::GetRenderer();

    TextureHandle handle = renderer->CreateTexture2DFromFile(filepath.c_str());

    if (!handle) return;

    auto& material = dynamic_cast<YunoRenderer*>(renderer)->GetMaterial(m_renderItem.materialHandle);

    switch (use)
    {
    case TextureUse::Albedo:
        material.albedo = handle;
        m_Albedo = handle;
        break;
    case TextureUse::Normal:
        material.normal = handle;
        m_Normal = handle;
        break;
    case TextureUse::Metallic:
        material.metallic = handle;
        break;
    case TextureUse::Roughness:
        material.roughness = handle;
        break;
    case TextureUse::AO:
        material.ao = handle;
        break;
    case TextureUse::ORM:
        material.orm = handle;
        m_Orm = handle;
        break;
    }
}

void Mesh::UpdateRenderItem(XMFLOAT4X4 mWorld)
{
    m_renderItem.Constant.world = mWorld;
}

void Mesh::SetObjectConstants(const Update_Data& constants)
{
    m_renderItem.Constant = constants;
}

void Mesh::Submit(XMFLOAT4X4& mWorld)
{
    XMStoreFloat4x4(&m_renderItem.Constant.world, XMLoadFloat4x4(&mWorld));
}

void Mesh::AnimSubmit(const std::vector<XMFLOAT4X4>& animTM)
{
    if (!m_renderItem.haveAnim) m_renderItem.haveAnim = true;
    for (size_t i = 0; i < animTM.size(); i++)
    {
        XMMATRIX mAnim = XMLoadFloat4x4(&animTM[i]);
        XMStoreFloat4x4(&m_renderItem.Constant.boneAnim[i], mAnim);
    }
}

void Mesh::LastSubmit()
{
    YunoEngine::GetRenderer()->Submit(m_renderItem);
}

void MeshNode::Submit(XMFLOAT4X4& mWorld)
{
    XMMATRIX world = mUserTM * XMLoadFloat4x4(&mWorld);
    XMFLOAT4X4 worldF;
    XMStoreFloat4x4(&worldF, world);

    for (auto& mesh : m_Meshs)
        mesh->Submit(worldF);

    for (auto& child : m_Childs)
        child->Submit(worldF);
}

void MeshNode::AnimSubmit(const std::vector<XMFLOAT4X4>& animTM)
{
    for (auto& mesh : m_Meshs)
        mesh->AnimSubmit(animTM);

    for (auto& child : m_Childs)
        child->AnimSubmit(animTM);
}

void MeshNode::LastSubmit()
{
    for (auto& mesh : m_Meshs)
        mesh->LastSubmit();

    for (auto& child : m_Childs)
        child->LastSubmit();
}
