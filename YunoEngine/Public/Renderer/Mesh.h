#pragma once
#include "RenderTypes.h"


class Mesh;

struct MeshNode {
    std::wstring m_name;
    MeshNode* m_Parent = nullptr;
    std::vector<std::unique_ptr<MeshNode>> m_Childs;

    XMFLOAT3 pos;
    XMFLOAT3 rot;
    XMFLOAT3 scale;

    std::vector<std::unique_ptr<Mesh>> m_Meshs;

    void Submit(XMFLOAT4X4& mWorld);
    void LastSubmit();
};

class Mesh
{
protected:
    std::string m_name;
    RenderItem                m_renderItem;
    MeshHandle              m_Mesh;
    MaterialHandle         m_Material;
    TextureHandle           m_Albedo;
    TextureHandle           m_Normal;
    TextureHandle           m_Orm;

    XMMATRIX m_mUser; //로컬->모델 이동행렬

public:
    Mesh();
    virtual ~Mesh();

    void SetName(const std::string& name) { m_name = name; }

    void Create(MeshHandle mesh, MaterialHandle mat, const XMMATRIX& modelTM);
    void Create(MeshHandle mesh, MaterialHandle mat, const XMFLOAT3& vPos, const XMFLOAT3& vRot, const XMFLOAT3& vScale);

    void SetMesh(MeshHandle mesh) { m_Mesh = mesh; }
    void SetMaterial(MaterialHandle mat) { m_Material = mat; }

    void UpdateRenderItem(XMFLOAT4X4 mWorld);

    void SetObjectConstants(const Update_Data& constants);


    void Submit(XMFLOAT4X4& mWorld);
    void LastSubmit();

    const RenderItem& GetRenderItem() { return m_renderItem; }

    const XMMATRIX& GetUserTM() { return m_mUser; }
};


