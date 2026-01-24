#include "pch.h"
#include "Tile.h"


VERTEX_Pos g_cubeMesh[] =
{
    //정면. ( Face#0) :   
    { -1.0, 0.0,-1.0} ,		//v0 : Position + Normal.★
    {  1.0, 0.0,-1.0} ,		//v1
    { -1.0,-2.0,-1.0} ,		//v2
    //       ( Face#1)
    {  1.0,-2.0,-1.0} ,		//v3

    //뒷면.  (Face#2) :  
    {  1.0, 0.0, 1.0} ,		//v4
    { -1.0, 0.0, 1.0} ,		//v5
    { -1.0,-2.0, 1.0} ,		//v6
    //		  (Face#3)
    {  1.0,-2.0, 1.0} ,		//v7


    // 우측면. (Face#4)
    {  1.0, 0.0,-1.0} ,		//v8
    {  1.0, 0.0, 1.0} ,		//v9
    {  1.0,-2.0,-1.0} ,		//v10
    //			(Face#5)
    {  1.0,-2.0, 1.0} ,		//v11


    // 좌측면. (Face#6)
    { -1.0, 0.0, 1.0} ,		//v12
    { -1.0, 0.0,-1.0} ,		//v13
    { -1.0,-2.0,-1.0} ,		//v14
    //			(Face#7)
    { -1.0,-2.0, 1.0} ,		//v15


    //상부.  ( Face#8)
    { -1.0, 0.0, 1.00} ,		//v16
    {  1.0, 0.0, 1.0} ,		//v17
    { -1.0, 0.0,-1.0} ,		//v18
    //		 ( Face#9)
    {  1.0, 0.0,-1.0} ,		//v19


    //하부.  ( Face#10)
    {  1.0,-2.0, 1.0} ,		//v20
    { -1.0,-2.0, 1.0} ,		//v21
    { -1.0,-2.0,-1.0} ,		//v22
    //		 ( Face#11)
    {  1.0,-2.0,-1.0}		//v23
};

VERTEX_Nrm g_cubeNrm[] =
{
    {0.0, 0.0,-1.0},
    {0.0, 0.0,-1.0},
    {0.0, 0.0,-1.0},

    {0.0, 0.0,-1.0},

    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},

    {0.0, 0.0, 1.0},

    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},

    {1.0, 0.0, 0.0},

    {-1.0, 0.0, 0.0},
    {-1.0, 0.0, 0.0},
    {-1.0, 0.0, 0.0},

    {-1.0, 0.0, 0.0},

    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},

    {0.0, 1.0, 0.0},

    {0.0,-1.0, 0.0},
    {0.0,-1.0, 0.0},
    {0.0,-1.0, 0.0},

    {0.0,-1.0, 0.0}
};

VERTEX_UV g_cubeUV[] =
{
    {0, 0},
    {1, 0},
    {0, 1},

    {1, 1},

    {0, 0},
    {1, 0},
    {1, 1},

    {0, 1},

    {0, 0},
    {1, 0},
    {0, 1},

    {1, 1},

    {0, 0},
    {1, 0},
    {1, 1},

    {0, 1},

    {0, 0},
    {1, 0},
    {0, 1},

    {1, 1},

    {0, 0},
    {1, 0},
    {1, 1},

    {0, 1}
};

INDEX g_cubeIndex[] =
{
    {0, 1, 2}, {2, 1, 3},		// 정면
    {4, 5, 7}, {7, 5, 6},		// 뒷면
    {8, 9, 10}, {10, 9, 11},	// 우측면
    {12, 13, 15}, {15, 13, 14},	// 좌측면
    {16, 17, 18}, {18, 17, 19},	// 상부
    {22, 23, 21}, {21, 23, 20},	//	하부
};

Tile::Tile()
{
}

Tile::~Tile()
{
}

bool Tile::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    if (!CreateMesh())
        return false;
    if (!CreateMaterial())
        return false;

    auto mesh = std::make_unique<Mesh>();


    XMMATRIX i = XMMatrixIdentity();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));

    m_MeshNode = std::make_unique<MeshNode>();

    m_MeshNode->m_Meshs.push_back(std::move(mesh));

    Backup();

    return true;
}

bool Tile::Update(float dTime)
{
    Unit::Update(dTime);
    return true;
}

bool Tile::Submit(float dTime)
{
    Unit::Submit(dTime);
    return true;
}

bool Tile::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_cubeMesh) / sizeof(VERTEX_Pos);
    streams.pos = g_cubeMesh;
    streams.nrm = g_cubeNrm;
    streams.uv = g_cubeUV;

    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_cubeIndex, _countof(g_cubeIndex));
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool Tile::CreateMaterial()
{
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/white.png");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::Basic;
    md.passKey.ps = ShaderId::Basic;
    md.passKey.vertexFlags = VSF_Pos | VSF_Nrm | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    md.albedo = m_Albedo;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    // 첫번째 머테리얼 생성
    m_defaultMaterial = m_pRenderer->CreateMaterial(md);
    if (m_defaultMaterial == 0)
        return false;

    return true;
}
