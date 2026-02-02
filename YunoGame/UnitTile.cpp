#include "pch.h"
#include "UnitTile.h"

#include "PlayQueue.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Tile
    {
        AutoReg_Tile()
        {
            ObjectTypeRegistry::Instance().Register(L"Tile", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<UnitTile>(d); });
        }
    } s_reg_Tile;
}

VERTEX_Pos g_cubeMesh[] =
{
    //정면. ( Face#0) :   
    { -0.5, 0.0,-0.5} ,		//v0 : Position + Normal.★
    {  0.5, 0.0,-0.5} ,		//v1
    { -0.5,-1.0,-0.5} ,		//v2
    //       ( Face#1)
    {  0.5,-1.0,-0.5} ,		//v3

    //뒷면.  (Face#2) :  
    {  0.5, 0.0, 0.5} ,		//v4
    { -0.5, 0.0, 0.5} ,		//v5
    { -0.5,-1.0, 0.5} ,		//v6
    //		  (Face#3)
    {  0.5,-1.0, 0.5} ,		//v7


    // 우측면. (Face#4)
    {  0.5, 0.0,-0.5} ,		//v8
    {  0.5, 0.0, 0.5} ,		//v9
    {  0.5,-1.0,-0.5} ,		//v10
    //			(Face#5)
    {  0.5,-1.0, 0.5} ,		//v11


    // 좌측면. (Face#6)
    { -0.5, 0.0, 0.5} ,		//v12
    { -0.5, 0.0,-0.5} ,		//v13
    { -0.5,-1.0,-0.5} ,		//v14
    //			(Face#7)
    { -0.5,-1.0, 0.5} ,		//v15


    //상부.  ( Face#8)
    { -0.5, 0.0, 0.5} ,		//v16
    {  0.5, 0.0, 0.5} ,		//v17
    { -0.5, 0.0,-0.5} ,		//v18
    //		 ( Face#9)
    {  0.5, 0.0,-0.5} ,		//v19


    //하부.  ( Face#10)
    {  0.5,-1.0, 0.5} ,		//v20
    { -0.5,-1.0, 0.5} ,		//v21
    { -0.5,-1.0,-0.5} ,		//v22
    //		 ( Face#11)
    {  0.5,-1.0,-0.5}		//v23
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



UnitTile::UnitTile()
{
    this->m_name = L"Tile";
}


UnitTile::~UnitTile()
{
}


bool UnitTile::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);

    if (!this->m_pInput || !this->m_pRenderer || !this->m_pTextures)
        return false;
    if (!CreateMesh())
        return false;
    if (!CreateMaterial())
        return false;

    auto mesh = std::make_unique<Mesh>();


    XMMATRIX i = XMMatrixIdentity();
    mesh->Create(this->m_defaultMesh, this->m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));

    this->m_MeshNode = std::make_unique<MeshNode>();

    this->m_MeshNode->m_Meshs.push_back(std::move(mesh));

    Backup();

    return true;
}


bool UnitTile::Update(float dTime)
{
    Unit::Update(dTime);
    return true;
}


bool UnitTile::Submit(float dTime)
{
    Unit::Submit(dTime);
    return true;
}


bool UnitTile::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_cubeMesh) / sizeof(VERTEX_Pos);
    streams.pos = g_cubeMesh;
    streams.nrm = g_cubeNrm;
    streams.uv = g_cubeUV;

    this->m_defaultMesh = this->m_pRenderer->CreateMesh(streams, g_cubeIndex, _countof(g_cubeIndex));
    if (this->m_defaultMesh == 0)
        return false;

    return true;
}


bool UnitTile::CreateMaterial()
{
    this->m_Albedo = this->m_pTextures->LoadTexture2D(L"../Assets/Textures/white.png");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::Basic;
    md.passKey.ps = ShaderId::Basic;
    md.passKey.vertexFlags = VSF_Pos | VSF_Nrm | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    md.albedo = this->m_Albedo;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    // 첫번째 머테리얼 생성
    this->m_defaultMaterial = this->m_pRenderer->CreateMaterial(md);
    if (this->m_defaultMaterial == 0)
        return false;

    return true;
}


void UnitTile::FlashColor(C3 color, int count, float diff, float speed)
{
    m_maskColor = color;
    m_count = count;
    m_diff = diff;
    m_FxSpeed = speed;
}


