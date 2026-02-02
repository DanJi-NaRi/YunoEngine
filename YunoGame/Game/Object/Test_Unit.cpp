#include "pch.h"
#include "Test_Unit.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Test_Unit //<- 이름은 자유
    {
        AutoReg_Test_Unit()
        {
            //아래줄 주석 풀고 이름만 바꿔주고 등록하면됩니다
            ObjectTypeRegistry::Instance().Register(L"Test_Unit", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Test_Unit>(d); });
        }
    } s_reg_Test_Unit;//<- 이름은 자유
}


VERTEX_Pos g_Test_Unit_pos[] = {
    { -1.0f,  1.0f,  0.0f },    // 좌상
    {  1.0f,  1.0f,  0.0f },    // 우상
    { -1.0f, -1.0f,  0.0f },    // 좌하
    {  1.0f, -1.0f,  0.0f }     // 우하
};

VERTEX_Nrm g_Test_Unit_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Test_Unit_uv[] =
{
    {  0.0f,  0.0f},
    {  1.0f,  0.0f},
    {  0.0f,  1.0f},
    {  1.0f,  1.0f}
};


INDEX g_Test_Unit_idx[] =
{
    { 0, 1, 2},
    { 2, 1, 3 }
};


Test_Unit::Test_Unit()
{
}

Test_Unit::~Test_Unit()
{
}


bool Test_Unit::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
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

bool Test_Unit::Update(float dTime)
{
    //게임 로직 업데이트 ㄱㄱ
    m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
    m_constant.roughRatio = 1.0f;
    m_constant.roughRatio = 1.0f;
    m_constant.shadowBias = 0.005f;

    m_time += dTime;


    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRotx = XMMatrixIdentity();
    XMMATRIX mRot = mRotx;
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM = mScale * mRot * mTrans;


    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);



    return true;
}

bool Test_Unit::Submit(float dTime)
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

bool Test_Unit::CreateMesh()
{


    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_Test_Unit_pos) / sizeof(VERTEX_Pos);
    streams.pos = g_Test_Unit_pos;
    streams.nrm = g_Test_Unit_nrm;
    streams.uv = g_Test_Unit_uv;

    // 옵셔널
    //streams.topology = Yuno_TRIANGLESTRIP;



    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_Test_Unit_idx, _countof(g_Test_Unit_idx));
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool Test_Unit::CreateMaterial()
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

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit1.png");
    md.albedo = m_Albedo;
    m_addMaterial1 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial1 == 0)
        return false;

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit2.png");
    md.albedo = m_Albedo;
    m_addMaterial2 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial2 == 0)
        return false;

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit3.png");
    md.albedo = m_Albedo;
    m_addMaterial3 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial3 == 0)
        return false;

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit4.png");
    md.albedo = m_Albedo;
    m_addMaterial4 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial4 == 0)
        return false;

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit5.png");
    md.albedo = m_Albedo;
    m_addMaterial5 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial5 == 0)
        return false;

    // 추가 머테리얼 생성
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Test/unit6.png");
    md.albedo = m_Albedo;
    m_addMaterial6 = m_pRenderer->CreateMaterial(md);
    if (m_addMaterial6 == 0)
        return false;

    return true;
}

