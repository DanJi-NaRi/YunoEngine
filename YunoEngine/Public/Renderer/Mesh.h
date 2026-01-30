#pragma once
#include "RenderTypes.h"

class Mesh;
class MeshDesc;

struct MeshNode {
    MeshNode() { mUserTM = XMMatrixIdentity(); }
    std::wstring m_name;
    MeshNode* m_Parent = nullptr;
    std::vector<std::unique_ptr<MeshNode>> m_Childs;

    XMMATRIX mUserTM;

    std::vector<std::unique_ptr<Mesh>> m_Meshs;

    void Submit(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos);
    void AnimSubmit(const std::vector<XMFLOAT4X4>& animTM);
    void LastSubmit();
};

enum class TextureUse
{
    Albedo = 0,
    Normal = 1,
    Metallic = 2,
    Roughness = 3,
    AO = 4,
    ORM = 5,    // AO, Metallic, Roughness
    Mask = 6,

    MAX
};

class Mesh
{
protected:
    std::string m_name;
    RenderItem              m_renderItem;
    MeshHandle              m_Mesh;
    MaterialHandle         m_Material;
    TextureHandle           m_Albedo;
    TextureHandle           m_Normal;
    TextureHandle           m_Orm;

    //XMMATRIX m_mUser; //로컬->모델 이동행렬

public:
    Mesh();
    virtual ~Mesh();

    void SetName(const std::string& name) { m_name = name; }

    void Create(MeshHandle mesh, MaterialHandle mat);
    void Create(MeshHandle mesh, MaterialHandle mat, const XMFLOAT3& vPos, const XMFLOAT3& vRot, const XMFLOAT3& vScale);

    void SetMesh(MeshHandle mesh) { m_Mesh = mesh; }
    void SetMaterial(MaterialHandle mat) { m_Material = mat; }

    void EmissiveOn() { m_renderItem.isEmissive = true; }
    void EmissiveOff() { m_renderItem.isEmissive = false; }

    void SetEmissiveColor(const XMFLOAT4& color);

    void CheckOption();

    void SetTexture(TextureUse use, const std::wstring& filepath);
    void SetMaskColor(const XMFLOAT4& col);
    void SetOpacity(const float opacity);

    void UpdateRenderItem(XMFLOAT4X4 mWorld);

    void SetObjectConstants(const Update_Data& constants);


    void Submit(const XMFLOAT4X4& mWorld, const XMFLOAT3& pos);
    void AnimSubmit(const std::vector<XMFLOAT4X4>& animTM);
    void LastSubmit();

    RenderItem& GetRenderItem() { return m_renderItem; }

    MeshDesc BuildDesc();

#ifdef _DEBUG
    virtual void Serialize(int num); //나중에 상속해서 새로운 오브젝트 만들 때 임구이에 띄우고 싶은거있으면 이 함수 오버라이드하면됌

    //임구이용 스트링
    std::string mat;
    std::string emissive;
    std::string emissiveCol;
#endif
};


