#include "pch.h"

#include "Mesh.h"
#include "IRenderer.h"
#include "YunoEngine.h"
#include "YunoRenderer.h"
#include "UImgui.h"

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

    CheckOption();
}

void Mesh::Create(MeshHandle mesh, MaterialHandle mat, const XMFLOAT3& vPos, const XMFLOAT3& vRot, const XMFLOAT3& vScale)
{
    m_Mesh = mesh;
    m_Material = mat;

    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;

    CheckOption();
}

void Mesh::SetEmissiveColor(const XMFLOAT4& color)
{
    m_renderItem.Constant.emissiveColor = color;
}

void Mesh::CheckOption()
{
    const auto& renderer = YunoEngine::GetRenderer();
    auto& material = dynamic_cast<YunoRenderer*>(renderer)->GetMaterial(m_renderItem.materialHandle);

    if (material.emissive)
        m_renderItem.isEmissive = true;
}

void Mesh::SetTexture(TextureUse use, const std::wstring& filepath)
{
    const auto& renderer = YunoEngine::GetRenderer();

    TextureHandle handle;
    
    if(filepath.find(L"Albedo") != std::wstring::npos)
        handle = renderer->CreateColorTexture2DFromFile(filepath.c_str());
    else
        handle = renderer->CreateDataTexture2DFromFile(filepath.c_str());

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

void Mesh::SetMaskColor(const XMFLOAT4& col)
{
    m_renderItem.Constant.baseColor = col;
}

void Mesh::SetOpacity(const float opacity)
{
    m_renderItem.Constant.opacity = opacity;
}

void Mesh::UpdateRenderItem(XMFLOAT4X4 mWorld)
{
    m_renderItem.Constant.world = mWorld;
}

void Mesh::SetObjectConstants(const Update_Data& constants)
{
    m_renderItem.Constant = constants;
}

void Mesh::Submit(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos)
{
    XMStoreFloat4x4(&m_renderItem.Constant.world, XMLoadFloat4x4(&mWorld));
    m_renderItem.Constant.worldPos = pos;
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

    // 
}

void MeshNode::Submit(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos)
{
    XMMATRIX world = mUserTM * XMLoadFloat4x4(&mWorld);
    XMFLOAT4X4 worldF;
    XMStoreFloat4x4(&worldF, world);

    for (auto& mesh : m_Meshs)
        mesh->Submit(worldF, pos);

    for (auto& child : m_Childs)
        child->Submit(worldF, pos);
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

#ifdef _DEBUG
void Mesh::Serialize(int num)
{
    std::string numstr = std::to_string(num);
    mat = "Material_" + numstr;
    if (UI::CollapsingHeader(mat.c_str()))
    {
        if (m_renderItem.isEmissive)
        {
            emissive = "Emissive_" + m_name + "##" + numstr;
            if (UI::CollapsingHeader(emissive.c_str()))
            {
                emissiveCol = "EmissiveColor##" + numstr;
                if (UI::DragFloat3Editable(emissiveCol.c_str(), (float*)&m_renderItem.Constant.emissiveColor, 0.001f, 0.0f, 1.0f))
                {

                }
            }
        }
    }
}
#endif
