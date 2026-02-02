#include "pch.h"
#include "Quad.h"


#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"


VERTEX_Pos g_Quad_pos[] = {
    { -1.0f,  1.0f,  0.0f },    // 좌상
    {  1.0f,  1.0f,  0.0f },    // 우상
    { -1.0f, -1.0f,  0.0f },    // 좌하
    {  1.0f, -1.0f,  0.0f }     // 우하
};

VERTEX_Nrm g_Quad_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Quad_uv[] =
{
    {  0.0f,  0.0f},
    {  1.0f,  0.0f},
    {  0.0f,  1.0f},
    {  1.0f,  1.0f}
};

VERTEX_UV g_Terrain_uv[] =
{
    {  0.0f,   0.0f },
    { 24.0f,   0.0f },
    {  0.0f,  24.0f},
    { 24.0f,  24.0f}
};

INDEX g_Quad_idx[] =
{
    { 0, 1, 2},
    { 2, 1, 3 }
};

namespace {
    struct AutoReg_Quad
    {
        AutoReg_Quad()
        {
            ObjectTypeRegistry::Instance().Register(L"Quad", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Quad>(d); });
        }
    } s_reg_Quad;
}


Quad::Quad()
{
    unitType = L"Quad";
}

Quad::~Quad()
{
}


bool Quad::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
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

    //mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    //m_Meshs.push_back(std::move(mesh));


    Backup();


    return true;
}

bool Quad::Update(float dTime)
{
    //게임 로직 업데이트 ㄱㄱ
    m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
    m_constant.roughRatio = 1.0f;
    m_constant.roughRatio = 1.0f;
    m_constant.shadowBias = 0.005f;

    m_time += dTime;

    m_vScale.x = 60;
    m_vScale.y = 60;

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRotx = XMMatrixRotationX(XMConvertToRadians(90));
    XMMATRIX mRot = mRotx;
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM = mScale * mRot * mTrans;


    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);



    return true;
}

bool Quad::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    Unit::Submit(dTime);


    return true;
}

bool Quad::CreateMesh()
{


    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_Quad_pos)/sizeof(VERTEX_Pos);
    streams.pos = g_Quad_pos;
    streams.nrm = g_Quad_nrm;
    streams.uv = g_Terrain_uv;

    // 옵셔널
    //streams.topology = Yuno_TRIANGLESTRIP;



    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_Quad_idx, _countof(g_Quad_idx));
    if (m_defaultMesh == 0)
        return false; 

    return true;    
}

bool Quad::CreateMaterial()
{

    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/woodbox.bmp");

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
    
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/Grass.jpg");

    // 추가 머테리얼 생성
    md.passKey.raster = RasterPreset::CullNone;
    md.albedo = m_Albedo;

    m_addMaterial = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial == 0)
        return false;

    return true;
}

