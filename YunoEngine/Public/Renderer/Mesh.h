#pragma once
#include "RenderTypes.h"

class Mesh
{
protected:
    RenderItem                m_renderItem;
    MeshHandle              m_Mesh;
    MaterialHandle         m_Material;
    TextureHandle           m_Albedo;
    TextureHandle           m_Normal;
    TextureHandle           m_Orm;

    //모델 공간 위치
    XMFLOAT3 m_vPos;
    XMFLOAT3 m_vRot;
    XMFLOAT3 m_vScale;

    XMFLOAT4X4 m_mUser; //로컬->모델 이동행렬

public:
    Mesh();
    virtual ~Mesh();

    void Create(MeshHandle mesh, MaterialHandle mat, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);

    void SetMesh(MeshHandle mesh) { m_Mesh = mesh; }
    void SetMaterial(MaterialHandle mat) { m_Material = mat; }

    void UpdateRenderItem(XMFLOAT4X4 mWorld);
    const RenderItem& GetRenderItem() { return m_renderItem; }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

    const XMFLOAT4X4& GetUserTM() { return m_mUser; }
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======

    const XMFLOAT4X4& GetUserTM() { return m_mUser; }
>>>>>>> 2e8bd5d (좌표계 수정)
=======

    const XMFLOAT4X4& GetUserTM() { return m_mUser; }
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
};


