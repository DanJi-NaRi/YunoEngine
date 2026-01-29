#include "pch.h"
#include "Image.h"

Image::Image(UIFactory& uiFactory) : Widget(uiFactory)
{
}

Image::~Image()
{
}

bool Image::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Widget::Create(name, id, vPos);
    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;


    if (!CreateMesh())
    {
        return false;
    }


    if (!CreateMaterial())
    {
        return false;
    }



    m_MeshNode = std::make_unique<MeshNode>();

    auto mesh = std::make_unique<Mesh>();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    m_MeshNode->m_Meshs.push_back(std::move(mesh));
     
    {
        m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
        m_constant.roughRatio = 1.0f;

        m_constant.metalRatio = 1.0f;
        m_constant.shadowBias = 0.005f;
    }
    Backup();

    return true;

}

bool Image::Start()
{
    return true;
}

bool Image::UpdateTransform(float dTime)
{
    Widget::UpdateTransform(dTime);

    return true;
}

bool Image::UpdateLogic(float dTime)
{
    //Widget::UpdateLogic(dTime);

    return true;
}

bool Image::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    Widget::Submit(dTime);


    return true;
}


bool Image::CreateMesh()
{
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용

    return true;
}


bool Image::CreateMaterial()
{
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/black.png");

    MaterialDesc md{};

    md.passKey.vs = ShaderId::UIBase;
    md.passKey.ps = ShaderId::UIBase;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::Off;


    md.albedo = m_Albedo;
    //md.albedo = 0;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    // 첫번째 머테리얼 생성
    m_defaultMaterial = m_pRenderer->CreateMaterial(md);
    if (m_defaultMaterial == 0)
        return false;


    //m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/Grass.jpg");

    // 추가 머테리얼 생성
    //md.passKey.raster = RasterPreset::CullNone;
    //md.albedo = m_Albedo;

    //m_addMaterial = m_pRenderer->CreateMaterial(md);
    //if (m_addMaterial == 0)
    //    return false;

    return true;
}

