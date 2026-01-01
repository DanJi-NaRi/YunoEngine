#include "pch.h"
#include "Triangle.h"
#include "IMesh.h"

VERTEX_Pos g_Triangle_pos[] = {
    {  0.0f,  0.5f,  0.0f},
    {  0.5f, -0.5f,  0.0f},
    { -0.5f, -0.5f,  0.0f}
};

VERTEX_Nrm g_Triangle_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Triangle_uv[] =
{
    {  0.0f,  1.0f},
    {  0.0f,  1.0f},
    {  0.0f,  1.0f}
};

INDEX g_Triangle_idx[] =
{
    { 0, 1, 2},
};

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

bool Triangle::Create(XMFLOAT3 vPos)
{
    Unit::Create(vPos);


    if (!CreateMesh())
        return false;

    if (!CreateMaterial())
        return false;

    Backup();

    return true;
}

bool Triangle::Update(float dTime)
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

bool Triangle::CreateMesh()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = 4;
    streams.pos = g_Triangle_pos;
    streams.nrm = g_Triangle_nrm;
    streams.uv = g_Triangle_uv;



    m_mesh = renderer->CreateMesh(streams, g_Triangle_idx, _countof(g_Triangle_idx));
    if (m_mesh == 0)
        return false;

    return true;
}

bool Triangle::CreateMaterial()
{
    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
        return false;

    m_material = renderer->CreateMaterial_Default();

    return true;
}