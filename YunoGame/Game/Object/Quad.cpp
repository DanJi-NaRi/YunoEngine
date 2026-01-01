#include "pch.h"
#include "Quad.h"

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"




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
    { 20.0f,   0.0f },
    {  0.0f,  20.0f},
    { 20.0f,  20.0f}
};

INDEX g_Quad_idx[] =
{
    { 0, 1, 2},
    { 2, 1, 3 }
};


Quad::Quad()
{
}

Quad::~Quad()
{
}

bool Quad::Create(XMFLOAT3 vPos)
{
    Unit::Create(vPos);


    if (!CreateMesh())
        return false;

    if (!CreateMaterial())
        return false;

    Backup();

    return true;
}

bool Quad::Update(float dTime)
{
    //게임 로직 업데이트 ㄱㄱ

    //m_vRot.y += dTime*3;

    m_vScale.x = 64;
    m_vScale.y = 64;

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRotx = XMMatrixRotationX(XMConvertToRadians(90));
    XMMATRIX mRot = mRotx;
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM = mScale * mRot * mTrans;


    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    //Unit::Update(dTime);

    return true;
}

bool Quad::CreateMesh()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = 4;
    streams.pos = g_Quad_pos;
    streams.nrm = g_Quad_nrm;
    streams.uv = g_Terrain_uv;



    m_mesh = renderer->CreateMesh(streams, g_Quad_idx, _countof(g_Quad_idx));
    if (m_mesh == 0)
        return false;

    return true;
}

bool Quad::CreateMaterial()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    ITextureManager* texMan = YunoEngine::GetTextureManager();
    if (!texMan)
        return false;

    m_Albedo = texMan->LoadTexture2D(L"../Assets/Textures/woodbox.bmp");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::Basic;
    md.passKey.ps = ShaderId::Basic;
    md.passKey.vertexFlags = VSF_Pos | VSF_Nrm | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::WireCullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    md.baseColor = { 1,1,1,1 };
    md.roughness = 1.0f;
    md.metallic = 0.0f;

    md.albedo = m_Albedo;
    md.normal = 0;
    md.orm = 0;

    m_material = renderer->CreateMaterial(md);
    if (m_material == 0)
        return false;

    return true;
}

