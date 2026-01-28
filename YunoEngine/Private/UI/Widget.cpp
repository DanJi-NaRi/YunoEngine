#include "pch.h"
#include "Mesh.h"
#include "YunoEngine.h"

#include "Widget.h"
#include "IWindow.h"

#include "IInput.h"
#include "UIManager.h"

VERTEX_Pos g_Widget_pos[] = {
    { 0,0,0 },    // 좌상
    { 1,0,0 },    // 우상
    { 0,1,0 },    // 좌하
    { 1,1,0 }     // 우하
};

XMVECTOR g_Widget_Quad[4] =
{
    XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
    XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f),
    XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
    XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f),
};
/*
VERTEX_Pos g_Widget_pos[] = {
    { -0.5f,0.5f,0 },    // 좌상
    { 0.5f,0.5f,0 },    // 우상
    { -0.5f,-0.5f,0 },    // 좌하
    { 0.5f,-0.5f,0 }     // 우하
};
*/

// UI 쿼드는 노말 안씀
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


Widget::Widget(UIManager* uiManager) : m_pUIManager(uiManager)
{
    Clear();

    m_zOrder = 0;

    m_defaultMesh = 0;
    m_defaultMaterial = 0;

    m_Albedo = 0;
    m_Normal = 0;
    m_Orm = 0;

    // 단순 출력 우선 기본값
    m_size.x = 100.0f;
    m_size.y = 100.0f;

    m_vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_vRot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

    m_spriteSize.x = 0;
    m_spriteSize.y = 0;

    m_canvasSize.x = 0;
    m_canvasSize.y = 0;

    m_finalSize.x = 0;
    m_finalSize.y = 0;

    m_finalSize.x = m_size.x * m_vScale.x;
    m_finalSize.y = m_size.y * m_vScale.y;

    m_anchor = UIDirection::LeftTop;
    m_pivot = PivotFromUIDirection(UIDirection::LeftTop);
}

Widget::~Widget()
{
    //ClearChild(); // 나중에 흐름 체크하고 넣기
    Clear();
}

bool Widget::CreateMesh()
{
    // 사실상 UI Widget은 사각형 고정이다.
    // 버튼 같은 경우, 쿼드를 이용한 다양한 모양의 히트테스트를 하려면
    // 그냥 알파 기반이나 스텐실 / 도형 수학 등을 이용해도 된다.
    // 정 필요하다면,  9-Slice 메쉬를 추가해도 되기는 한다.

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_UV;
    streams.vtx_count = sizeof(g_Widget_pos) / sizeof(VERTEX_Pos);
    streams.pos = g_Widget_pos;
    //streams.nrm = g_Widget_nrm;
    streams.uv = g_Widget_uv;



    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_Widget_idx, _countof(g_Widget_idx));
    if (m_defaultMesh == 0)
        return false;

    return true;
}

bool Widget::CreateMaterial(std::wstring path, MaterialDesc* pDesc)
{
    m_Albedo = m_pTextures->LoadTexture2D(path.c_str());

    MaterialDesc md{};
    if (pDesc) {
        md =  *pDesc;
    }
    else {
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
    }
    
    // 첫번째 머테리얼 생성
    m_defaultMaterial = m_pRenderer->CreateMaterial(md);
    if (m_defaultMaterial == 0) return false;

    return true;
}

bool Widget::AddMaterial(std::wstring path)
{
    int beforeSize = m_addMaterial.size();

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

    m_addMaterial.push_back(m_pRenderer->CreateMaterial(md));
    if (beforeSize == m_addMaterial.size()) return false;
    
    return false;
}


bool Widget::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    m_id = id;
    m_name = name;

    m_vPos = vPos;
    //m_vRot = vRot;
    //m_vScale = vScale;

    m_spriteSize.x = (float)50;
    m_spriteSize.y = (float)50;

    // 테스트용 - 초기 생성 시 스프라이트 사이즈와 동일하게 
    // (추후 에디터 기능으로 flag 추가 가능)

    m_size.x = m_spriteSize.x;
    m_size.y = m_spriteSize.y;
    
    //m_finalSize.x = m_vScale.x * m_width;
    //m_finalSize.y = m_vScale.y * m_height;

    //m_pivot = PivotFromUIDirection(UIDirection::LeftTop);

    // Rect 갱신은 Update()에서
    Widget::Update();

    return true;
}

bool Widget::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale)
{
    m_pRenderer = YunoEngine::GetRenderer();
    m_pTextures = YunoEngine::GetTextureManager();
    m_pInput = YunoEngine::GetInput();

    m_id = id;
    m_name = name;

    m_vPos = vPos;
    m_vRot = vRot;
    m_vScale = vScale;

    m_spriteSize.x = (float)50;
    m_spriteSize.y = (float)50;

    // 테스트용 - 초기 생성 시 스프라이트 사이즈와 동일하게 
    // (추후 에디터 기능으로 flag 추가 가능)
    m_size.x = m_spriteSize.x;
    m_size.y = m_spriteSize.y;

    m_canvasSize.x = 0;
    m_canvasSize.y = 0;

    m_finalSize.x = m_vScale.x * m_size.x;
    m_finalSize.y = m_vScale.y * m_size.y;

    //m_pivot = PivotFromUIDirection(UIDirection::LeftTop);

    // Rect 갱신은 Update()에서
    Widget::Update();

    return true;
}


static bool tst = false;

bool Widget::Update(float dTime)
{
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vRot.z -= 1 * dTime; } // 회전 디버깅 [
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vRot.z += 1 * dTime; } // 회전 디버깅 ]
    if (m_pInput->IsKeyPressed(VK_OEM_7)) {                       // 피벗 디버깅 '
        tst = !tst;
        std::cout << "pressed!" << std::endl;

        (tst) ? SetPivot(UIDirection::LeftTop) : SetPivot(UIDirection::Center);
    }

    // DX 의 레스터라이즈 규칙에 따른 2D 픽셀좌표 보정.
    //m_vPos.x -= 0.5f;	m_vPos.y -= 0.5f;

    m_clientSize = Float2((float)YunoEngine::GetWindow()->GetClientWidth(),
                         (float)YunoEngine::GetWindow()->GetClientHeight());

    
   
        Float2 origin = Float2(1920.0f, 1080.0f); // 기준(디자인) 해상도
        Float2 canvas = m_pUIManager->GetCanvasSize(); // 현재 클라이언트/캔버스

        float sx = canvas.x / origin.x;
        float sy = canvas.y / origin.y;

        // 레터박스(전체가 보이도록) => 더 작은 스케일 채택
        float s = (sx < sy) ? sx : sy;

        // 16:9 유효영역(스케일 적용 후 origin 크기)
        Float2 fitted = Float2(origin.x * s, origin.y * s);

        // 남는 공간(레터박스) 분배: 중앙 정렬
        Float2 letterboxOffset = Float2(
            (canvas.x - fitted.x) * 0.5f,
            (canvas.y - fitted.y) * 0.5f);

        m_canvasOffset = Float2(s, s);
        Float2 m_canvasLetterboxOffset = letterboxOffset; // 이동

        XMFLOAT3 finalSize;
        finalSize.x = m_size.x * m_vScale.x * m_canvasOffset.x;
        finalSize.y = m_size.y * m_vScale.y * m_canvasOffset.y;
        finalSize.z = 1.0f;

        XMFLOAT3 finalPos;
        finalPos.x = m_vPos.x * m_canvasOffset.x + m_canvasLetterboxOffset.x;
        finalPos.y = m_vPos.y * m_canvasOffset.y + m_canvasLetterboxOffset.y;
        finalPos.z = m_vPos.z;


    m_finalSize = Float2(m_size.x * m_vScale.x, m_size.y * m_vScale.y);

    //m_vScale.z = 1.0f; // UI는 z scale 의미 없음(일단 1로 고정)

    //XMMATRIX mScale = XMMatrixScaling(m_finalSize.x, m_finalSize.y, 1.0f);
    XMMATRIX mScale = XMMatrixScaling(finalSize.x, finalSize.y, 1.0f);
    XMMATRIX mRot =   XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    //XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z); // 스크린 좌표 - 픽셀 기준(z는 사용 안함)
    XMMATRIX mTrans = XMMatrixTranslation(finalPos.x, finalPos.y, m_vPos.z); // 스크린 좌표 - 픽셀 기준(z는 사용 안함)
    XMMATRIX mPivot = XMMatrixTranslation(-m_pivot.x, -m_pivot.y, 0.0f); // 피벗

    XMMATRIX mTM;

    if (m_Parent)
        mTM = mPivot * mScale * mRot * mTrans * m_Parent->GetWorldMatrix();
    else
        mTM = mPivot * mScale * mRot * mTrans;

    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    UpdateRect(); // m_rect 갱신

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

void Widget::UpdateRect()
{
    // 회전 무시된 Rect 크기
    float left = m_vPos.x - (m_pivot.x * m_finalSize.x);
    float top = m_vPos.y - (m_pivot.y * m_finalSize.y);
    float right = left + m_finalSize.x;
    float bottom = top + m_finalSize.y;

    // 뒤집기(음수 스케일) 대비
    float minX = (left < right) ? left : right;
    float maxX = (left > right) ? left : right;
    float minY = (top < bottom) ? top : bottom;
    float maxY = (top > bottom) ? top : bottom;

    m_rect =
    {
        (LONG)std::floor(minX),
        (LONG)std::floor(minY),
        (LONG)std::ceil(maxX),
        (LONG)std::ceil(maxY)
    };
}

bool Widget::IsCursorOverWidget(POINT mouseXY) // 마우스 커서가 위젯 위에 있는지 체크
{
    return PtInRect(&m_rect, mouseXY);
}

void Widget::SetMesh(std::unique_ptr<MeshNode>&& mesh)
{
    m_MeshNode = std::move(mesh);
}

void Widget::Attach(Widget* widget) //this가 부모, 파라미터로 자식
{
    if (widget->m_Parent)//기존 부모있으면 떨어진 후 결합
        widget->DettachParent();

    widget->m_Parent = this;

    m_Childs.insert(std::make_pair(widget->GetID(), widget));
}

void Widget::DettachParent()
{
    m_Parent->DettachChild(m_id);
}

void Widget::DettachChild(uint32_t id)
{
    if (m_Childs.find(id) == m_Childs.end())
        return;

    m_Childs.erase(id);
}

void Widget::ClearChild()
{
    if (m_Childs.empty())
        return;

    for (auto& [id, child] : m_Childs)
    {
        child->DettachParent();
    }

    m_Childs.clear();
}

void Widget::Clear() {
    m_pRenderer = nullptr;
    m_pTextures = nullptr;
    m_pInput = nullptr;
}

/// <summary>
/// 전역 변수 함수들...
/// </summary>
MeshHandle g_defaultWidgetMesh;

bool SetupDefWidgetMesh(MeshHandle& meshHandle, IRenderer* renderer)
{
    // UI의 기본 쿼드 생성. 대부분의 UI는 엔진단에서 이 작업 한 번 하고 GetGlobalMesh()만 사용하면 된다.

    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_UV;
    streams.vtx_count = sizeof(g_Widget_pos) / sizeof(VERTEX_Pos);
    streams.pos = g_Widget_pos;
    //streams.nrm = g_Widget_nrm;
    streams.uv = g_Widget_uv;

    meshHandle = renderer->CreateMesh(streams, g_Widget_idx, _countof(g_Widget_idx));
    if (meshHandle == 0) return false;

    return true;
}


