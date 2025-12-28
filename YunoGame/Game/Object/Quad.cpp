#include "pch.h"

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"

#include "Quad.h"

VERTEX_Pos g_Quad_pos[] = {
    { -0.5f,  0.5f,  0.0f },    // 좌상
    {  0.5f,  0.5f,  0.0f },    // 우상
    { -0.5f, -0.5f,  0.0f },    // 좌하
    {  0.5f, -0.5f,  0.0f }     // 우하
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

INDEX g_Quad_idx[] =
{
    { 0, 2, 1},
    { 1, 2, 3 }
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


    if (!CreateQuadMesh())
        return false;

    if (!CreateQuadMaterial())
        return false;

    Backup();

    return true;
}

bool Quad::Update(float dTime)
{
    //게임 로직 업데이트 ㄱㄱ

    m_vRot.y += dTime*3;

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationY(m_vRot.y);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM = mScale * mRot * mTrans;


    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    //Unit::Update(dTime);

    return true;
}

bool Quad::CreateQuadMesh()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = 4;
    streams.pos = g_Quad_pos;
    streams.nrm = g_Quad_nrm;
    streams.uv = g_Quad_uv;



    m_mesh = renderer->CreateMesh(streams, g_Quad_idx, _countof(g_Quad_idx));
    if (m_mesh == 0)
        return false;

    return true;
}

bool Quad::CreateQuadMaterial()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    m_material = renderer->CreateMaterial_Default();

    return true;
}
