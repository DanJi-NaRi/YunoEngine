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

bool Image::Update(float dTime)
{
    Widget::Update(dTime);

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


    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/Grass.jpg");

     //추가 머테리얼 생성
    md.passKey.raster = RasterPreset::CullNone;
    md.albedo = m_Albedo;

    MaterialHandle mtrl = m_pRenderer->CreateMaterial(md);
    m_materials.push_back(mtrl);
    if (mtrl != m_materials.back())
        return false;

    return true;
}

