#include "pch.h"
#include "WidgetTile.h"

#include "PlayQueue.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"


VERTEX_Pos g_qMesh[] =
{
    {-0.5, -0.5, 0.0},
    { 0.5, -0.5, 0.0},
    { 0.5,  0.5, 0.0},
    {-0.5,  0.5, 0.0}
};
VERTEX_UV g_qUV[] =
{
    {0.0, 0.0},
    {1.0, 0.0},
    {1.0, 1.0},
    {0.0, 1.0}
};
INDEX g_qIndex[]
{
    {0, 1, 2},
    {0, 2, 3}
};



WidgetTile::WidgetTile(UIFactory& uiFactory) : Widget(uiFactory)
{
    m_name = L"WidgetTile";
}

WidgetTile::~WidgetTile()
{
}


bool WidgetTile::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

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


bool WidgetTile::Update(float dTime)
{
    Widget::Update(dTime);
    return true;
}


bool WidgetTile::Submit(float dTime)
{
    Widget::Submit(dTime);
    return true;
}


bool WidgetTile::CreateMesh()
{
    m_defaultMesh = m_pRenderer->GetQuadMesh();
    if (m_defaultMesh == 0)
        return false;

    return true;
}


bool WidgetTile::CreateMaterial()
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


void WidgetTile::FlashColor(C3 color, int count, float diff, float speed)
{
    m_maskColor = color;
    m_count = count;
    m_diff = diff;
    m_FxSpeed = speed;
}


