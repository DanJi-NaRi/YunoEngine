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

public:
    Mesh();
    virtual ~Mesh();

    void Create(MeshHandle mesh, MaterialHandle mat);

    void UpdateRenderItem(XMFLOAT4X4 mWorld);
    const RenderItem& GetRenderItem() { return m_renderItem; }
};


