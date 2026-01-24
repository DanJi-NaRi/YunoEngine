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
    GridFactory::GetGridInfo(m_row, m_column);
    m_lineVtx.clear();
    m_lineVtx.reserve((m_row + 1) * 2 + (m_column + 1) * 2);

    float y = 0.0f;

    // 전체 그리드의 반 길이 (셀 개수 기준!)
    float halfX = m_column * 0.5f;
    float halfZ = m_row * 0.5f;

    // 세로선(= Z방향으로 뻗는 선): x가 변하고 z는 [-halfZ, +halfZ]
    for (int i = 0; i <= m_column; ++i) {             // column+1개 라인
        float x = -halfX + i;          // 일정 간격으로
        m_lineVtx.push_back({ x, y, -halfZ });
        m_lineVtx.push_back({ x, y,  halfZ });
    }

    // 가로선(= X방향으로 뻗는 선): z가 변하고 x는 [-halfX, +halfX]
    for (int i = 0; i <= m_row; ++i) {               // row+1개 라인
        float z = -halfZ + i;
        m_lineVtx.push_back({ -halfX, y, z });
        m_lineVtx.push_back({ halfX, y, z });
    }

    VertexStreams streams{};
    streams.flags = VSF_Pos;
    streams.vtx_count = m_lineVtx.size();
    streams.pos = m_lineVtx.data();
    streams.topology = Yuno_LINELIST;

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
