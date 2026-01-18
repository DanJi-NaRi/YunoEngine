#include "pch.h"
#include "Mesh.h"

#include "Widget.h"

VERTEX_Pos g_Widget_pos[] = {
    { -0.5f,  0.5f,  0.0f },    // 좌상
    {  0.5f,  0.5f,  0.0f },    // 우상
    { -0.5f, -0.5f,  0.0f },    // 좌하
    {  0.5f, -0.5f,  0.0f }     // 우하
};

VERTEX_Nrm g_Widget_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Widget_uv[] =
{
    {  0.0f,  0.0f},
    {  1.0f,  0.0f},
    {  0.0f,  1.0f},
    {  1.0f,  1.0f}
};

INDEX g_Widget_idx[] =
{
    { 0, 1, 2},
    { 2, 1, 3 }
};


Widget::Widget()
{
    m_zOrder = 0;

    m_defaultMesh = 0;
    m_defaultMaterial = 0;
    m_Albedo = 0;
    m_Normal = 0;
    m_Orm = 0;

    // 단순 출력 우선 기본값
    m_width = 100.0f;
    m_height = 100.0f;

    m_vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_vRot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Widget::~Widget()
{

}

bool Widget::CreateMesh()
{
    // 사실상 UI Widget은 사각형 고정이다.
    // 버튼 같은 경우, 쿼드를 이용한 다양한 모양의 히트테스트를 하려면
    // 그냥 알파 기반이나 스텐실 / 도형 수학 등을 이용해도 된다.
    // 정 필요하다면,  9-Slice 메쉬를 추가해도 되기는 한다.

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_Widget_pos) / sizeof(VERTEX_Pos);
    streams.pos = g_Widget_pos;
    streams.nrm = g_Widget_nrm;
    streams.uv = g_Widget_uv;



    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_Widget_idx, _countof(g_Widget_idx));
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool Widget::Create(XMFLOAT3 vPos)
{

    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    Widget::Update();

    return true;
}

bool Widget::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    m_id = id;
    m_name = name;
    //float sizex = (float)spr.Width;  //원본 스프라이트 크기 (Pixel)
    //float sizey = (float)spr.Height;

    m_spriteSizeX = (float)500;
    m_spriteSizeY = (float)500;

    // 테스트용 - 초기 생성 시 스프라이트 사이즈와 동일하게 
    // (추후 에디터 기능으로 flag 추가 가능)
    m_width = m_spriteSizeX;
    m_height = m_spriteSizeY;

    Create(vPos);

    return true;
}

bool Widget::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale)
{
    m_id = id;
    m_name = name;

    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    Widget::Update();

    return true;
}

bool Widget::Update(float dTime)
{
    // UI 위젯은 기본 쿼드(-0.5~0.5)에 대해 scale로 실제 픽셀 크기를 만든다.
    // 단순 출력 우선: (오쏘 1유닛=1픽셀) 가정
    // 만약 렌더러/UI 카메라에서 픽셀-유닛 환산이 필요하면 이 값만 조정하면 된다.
    const float UI_UNIT_PER_PX = 1.0f;

    //m_vScale.x = m_width * UI_UNIT_PER_PX;
    //m_vScale.y = m_height * UI_UNIT_PER_PX;
    m_vScale.x = m_spriteSizeX;
    m_vScale.y = m_spriteSizeY;
    m_vScale.z = 1.0f; // UI는 z scale 의미 없음(일단 1로 고정)

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z); // 스크린 좌표 - 픽셀 기준(z는 사용 안함)

    XMMATRIX mTM;

    if (m_Parent)
        mTM = mScale * mRot * mTrans * m_Parent->GetWorldMatrix();
    else
        mTM = mScale * mRot * mTrans;

    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    return true;
}

void Widget::Backup()
{
    m_vPosBk = m_vPos;
    m_vRotBk = m_vRot;
    m_vScaleBk = m_vScale;
    //m_vDirBk = m_vDir;
}

bool Widget::Submit(float dTime)
{

    m_MeshNode->Submit(m_mWorld);


    LastSubmit(dTime);

    return true;
}

bool Widget::LastSubmit(float dTime /*= 0*/)
{
    m_MeshNode->LastSubmit();

    return true;
}

void Widget::SetMesh(std::unique_ptr<MeshNode>&& mesh)
{
    m_MeshNode = std::move(mesh);
}

/// <summary>
/// 전역 변수 함수들...
/// </summary>
MeshHandle g_defaultWidgetMesh;

bool SetupDefWidgetMesh(MeshHandle& meshHandle, IRenderer* renderer)
{
    // UI의 기본 쿼드 생성. 대부분의 UI는 엔진단에서 이 작업 한 번 하고 GetGlobalMesh()만 사용하면 된다.

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_Widget_pos) / sizeof(VERTEX_Pos);
    streams.pos = g_Widget_pos;
    streams.nrm = g_Widget_nrm;
    streams.uv = g_Widget_uv;

    meshHandle = renderer->CreateMesh(streams, g_Widget_idx, _countof(g_Widget_idx));
    if (meshHandle == 0)
        return false;

    return true;
}
