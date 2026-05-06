#include "pch.h"

#include "Mesh.h"
#include "IRenderer.h"
#include "YunoEngine.h"
#include "YunoRenderer.h"
#include "UImgui.h"
#include "SerializeScene.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

std::unique_ptr<Mesh> Mesh::Clone() const
{
    auto clone = std::make_unique<Mesh>();
    clone->m_name = m_name;
    clone->m_renderItem = m_renderItem;
    clone->m_Mesh = m_Mesh;
    clone->m_Material = m_Material;
    clone->m_Albedo = m_Albedo;
    clone->m_Normal = m_Normal;
    clone->m_Orm = m_Orm;
#ifdef _DEBUG
    clone->mat = mat;
    clone->emissive = emissive;
    clone->emissiveCol = emissiveCol;
#endif
    return clone;
}

std::unique_ptr<MeshNode> MeshNode::Clone() const
{
    auto clone = std::make_unique<MeshNode>();
    clone->m_name = m_name;
    clone->mUserTM = mUserTM;

    clone->m_Meshs.reserve(m_Meshs.size());
    for (const auto& mesh : m_Meshs)
        clone->m_Meshs.push_back(mesh->Clone());

    clone->m_Childs.reserve(m_Childs.size());
    for (const auto& child : m_Childs)
    {
        auto childClone = child->Clone();
        childClone->m_Parent = clone.get();
        clone->m_Childs.push_back(std::move(childClone));
    }

    return clone;
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

void Mesh::SetDissolveAmount(float amount)
{
    if (!m_renderItem.isDissolve) m_renderItem.isDissolve = true;
    m_renderItem.Constant.dissolveAmount = amount;
}

void Mesh::SetDissolveWidth(float width)
{
    if (!m_renderItem.isDissolve) m_renderItem.isDissolve = true;
    m_renderItem.Constant.dissolveEdgeWidth = width;
}

void Mesh::SetDissolveColor(const XMFLOAT3& col)
{
    if (!m_renderItem.isDissolve) m_renderItem.isDissolve = true;
    m_renderItem.Constant.dissolveColor = col;
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
    
    if(TextureUse::Albedo == use)
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
    case TextureUse::Custom:
        material.custom.push_back(handle);
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
    m_renderItem.materialHandle = m_Material;
    m_renderItem.meshHandle = m_Mesh;

    XMStoreFloat4x4(&m_renderItem.Constant.world, XMLoadFloat4x4(&mWorld));
    m_renderItem.Constant.worldPos = pos;
}
void Mesh::SubmitWidget(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos, const Update_Data& updateData)
{
    //m_renderItem.Constant = updateData;
    m_renderItem.isWidget = true;

    XMStoreFloat4x4(&m_renderItem.Constant.world, XMLoadFloat4x4(&mWorld));
    m_renderItem.Constant.worldPos = pos;

    m_renderItem.Constant.widgetSize = updateData.widgetSize;
    m_renderItem.Constant.widgetValueFloat = updateData.widgetValueFloat;
    m_renderItem.Constant.widgetValueInt = updateData.widgetValueInt;
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

// 위젯용
void MeshNode::SubmitWidget(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos, const Update_Data& updateData)
{
    XMMATRIX world = mUserTM * XMLoadFloat4x4(&mWorld);
    XMFLOAT4X4 worldF;
    XMStoreFloat4x4(&worldF, world);

    for (auto& mesh : m_Meshs)
        mesh->SubmitWidget(worldF, pos, updateData);

    for (auto& child : m_Childs)
        child->SubmitWidget(worldF, pos, updateData);
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

MeshDesc Mesh::BuildDesc()
{
    MeshDesc md;

    md.emissive = m_renderItem.Constant.emissive;
    md.emissiveCol = FromXM(m_renderItem.Constant.emissiveColor);

    return md;
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
                if (UI::DragFloatEditable(("Power##" + numstr).c_str(), &m_renderItem.Constant.emissive, 0.01f, 0.0f, 50.0f))
                {

                }
            }
        }
    }
}
#endif
