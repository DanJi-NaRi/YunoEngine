#include "pch.h"
#include "WidgetGridLine.h"
#include "GridFactory.h"
#include "Minimap.h"

WidgetGridLine::WidgetGridLine(UIFactory& uiFactory) : Widget(uiFactory)
{
}

WidgetGridLine::~WidgetGridLine()
{
}

bool WidgetGridLine::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

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

void WidgetGridLine::CreateChild()
{
}

bool WidgetGridLine::Start()
{
    return true;
}

bool WidgetGridLine::Update(float dTime)
{
    Widget::Update(dTime);
    return true;
}

bool WidgetGridLine::Submit(float dTime)
{
    Widget::Submit(dTime);
    return true;
}

bool WidgetGridLine::CreateMesh()
{
    int m_row, m_column;
    GridFactory::GetGridInfo(m_row, m_column);
    m_lineVtx.clear();
    m_lineVtx.reserve((m_row + 1) * 2 + (m_column + 1) * 2);

    float z = 0.0f;

    // 전체 그리드의 반 길이 (셀 개수 기준!)
    float halfX = m_column * 0.5f;
    float halfY = m_row * 0.5f;


    // 가로선(= X방향으로 뻗는 선): z가 변하고 x는 [-halfX, +halfX]
    for (int i = 0; i <= m_row; ++i) {               // row+1개 라인
        float y = -halfY + i;
        m_lineVtx.push_back({ -halfX, y, z });
        m_lineVtx.push_back({ halfX, y, z });
    }

    // 세로선(= Y방향으로 뻗는 선): x가 변하고 y는 [-halfY, +halfY]
    for (int i = 0; i <= m_column; ++i) {             // column+1개 라인
        float x = -halfX + i;          // 일정 간격으로
        m_lineVtx.push_back({ x, -halfY, z });
        m_lineVtx.push_back({ x, -halfY, z });
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

