#include "pch.h"
#include "Triangle.h"

#include <IInput.h>
#include "YunoEngine.h"


VERTEX_Pos g_Triangle_pos[] = {
    {  0.0f,  1.0f,  0.0f},
    {  0.5f,  0.0f,  0.0f},
    { -0.5f,  0.0f,  0.0f}
};

VERTEX_Nrm g_Triangle_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Triangle_uv[] =
{
    {  0.5f,  0.0f}, // 상
    {  0.0f,  1.0f}, // 좌
    {  1.0f,  1.0f}  // 우
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


bool Triangle::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    if (!CreateMesh())
        return false;
    if (!CreateMaterial())
        return false;

    auto mesh = std::make_unique<Mesh>();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    m_MeshNode->m_Meshs.push_back(std::move(mesh));

    Backup();
}

bool Triangle::Update(float dTime)
{
    //게임 로직 업데이트 ㄱㄱ
    m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
    m_constant.roughRatio = 1.0f;
    m_constant.roughRatio = 1.0f;
    m_constant.shadowBias = 0.005f;


    if (m_pInput)
    {
        const float speed = 5.0f;            
        const float step = speed * dTime;

        if (m_pInput->IsKeyDown(VK_UP)) m_vPos.z += step;
        if (m_pInput->IsKeyDown(VK_DOWN)) m_vPos.z -= step;
        if (m_pInput->IsKeyDown(VK_LEFT)) m_vPos.x -= step;
        if (m_pInput->IsKeyDown(VK_RIGHT)) m_vPos.x += step;
    }

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

bool Triangle::Submit(float dTime)
{
    Unit::Submit(dTime);


    return true;
}

bool Triangle::CreateMesh()
{

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = 3;
    streams.pos = g_Triangle_pos;
    streams.nrm = g_Triangle_nrm;
    streams.uv = g_Triangle_uv;



    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_Triangle_idx, _countof(g_Triangle_idx));
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool Triangle::CreateMaterial()
{


    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/Grass.jpg");

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
