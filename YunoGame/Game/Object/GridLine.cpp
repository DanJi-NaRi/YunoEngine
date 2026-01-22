#include "pch.h"
#include "GridFactory.h"
#include "GridLine.h"

GridLine::GridLine()
{
}

GridLine::~GridLine()
{
}

bool GridLine::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
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

    Backup();


    return true;
}

bool GridLine::Update(float dTime)
{
    Unit::Update(dTime);
    return true;
}

bool GridLine::Submit(float dTime)
{
    Unit::Submit(dTime);
    return true;
}

bool GridLine::CreateMesh()
{
    int m_row, m_column;
    float m_cellSizeX, m_cellSizeZ;
    GridFactory::GetGridInfo(m_row, m_column, m_cellSizeX, m_cellSizeZ);
    int m_halfX = std::floor((m_column + 1) / 2.f);
    int m_halfZ = std::floor((m_row + 1) / 2.f);

    m_lineVtx.reserve((m_row + 1) * 2 + (m_column + 1) * 2);

    float y = 1.f;
    float halfLength = ((m_row) / 2.f) * m_cellSizeZ;
    int idx = 0;
    for (int i = -m_halfX; i < m_halfX; i++)
    {
        float x = i * m_cellSizeX;
        m_lineVtx.push_back({ x, y, -halfLength });
        m_lineVtx.push_back({ x, y, halfLength });
    }

    halfLength = ((m_column) / 2.f) * m_cellSizeX;
    for (int i = -m_halfZ; i < m_halfZ; i++)
    {
        float z = i * m_cellSizeZ;
        m_lineVtx.push_back({ -halfLength, y, z });
        m_lineVtx.push_back({ halfLength, y, z });
    }

    VertexStreams streams{};
    streams.flags = VSF_Pos;
    streams.vtx_count = m_lineVtx.size();
    streams.pos = m_lineVtx.data();

    m_defaultMesh = m_pRenderer->CreateMesh(streams, nullptr, 0);
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool GridLine::CreateMaterial()
{
    MaterialDesc md{};
    md.passKey.vs = ShaderId::DebugGrid;
    md.passKey.ps = ShaderId::DebugGrid;
    md.passKey.vertexFlags = VSF_Pos;

    md.passKey.blend = BlendPreset::Opaque;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    m_defaultMaterial = m_pRenderer->CreateMaterial(md);

    return true;
}
