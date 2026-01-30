#include "pch.h"
#include "Letterbox.h"

#include "Widget.h"
#include "IInput.h"
#include "YunoEngine.h"
#include "UIFactory.h"

Letterbox::Letterbox(UIFactory& uiFactory) : Image(uiFactory) {}
Letterbox::~Letterbox() {}

bool Letterbox::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Image::Create(name, id, vPos);
    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;

    if (!CreateMesh())
        return false;

    if (!CreateMaterial())
        return false;

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


bool Letterbox::UpdateTransform(float dTime)
{
    //Widget::UpdateTransform(dTime);

    // DX 의 레스터라이즈 규칙에 따른 2D 픽셀좌표 보정.
    //m_vPos.x -= 0.5f;	m_vPos.y -= 0.5f;

    //m_clientSize = m_uiFactory.GetCanvasSize();

    /*
    {
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
    }
     */

    m_finalSize = Float2(m_size.x * m_vScale.x, m_size.y * m_vScale.y);

    //m_vScale.z = 1.0f; // UI는 z scale 의미 없음(일단 1로 고정)

    XMMATRIX mScale = XMMatrixScaling(m_finalSize.x, m_finalSize.y, 1.0f);
    XMMATRIX mRot = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z); // 스크린 좌표 - 픽셀 기준(z는 사용 안함)
    //XMMATRIX mTrans = XMMatrixTranslation(finalPos.x, finalPos.y, m_vPos.z); // 스크린 좌표 - 픽셀 기준(z는 사용 안함)
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

bool Letterbox::UpdateLogic(float dTime)
{
    UpdateLetterBox(dTime);
    return true;
}


bool Letterbox::Submit(float dTime)
{
    Image::Submit(dTime);
    return true;
}

bool Letterbox::UpdateLetterBox(float dTime)
{
    //////////////////////////////////////////
    // 부모 Update(입력/애니메이션 등)
    //Image::Update(dTime);

    // 1) 캔버스 크기
    XMFLOAT2 C = { m_uiFactory.GetCanvasSize().x, m_uiFactory.GetCanvasSize().y };
    float Cw = C.x;
    float Ch = C.y;

    // 2) 레터박스 파라미터 계산 (origin이 화면 안에 "다 보이게" => min)
    float sx = Cw / m_origin.x;
    float sy = Ch / m_origin.y;
    float s = (sx < sy) ? sx : sy;

    float fittedW = m_origin.x * s;
    float fittedH = m_origin.y * s;

    float dx = (Cw - fittedW) * 0.5f; // 좌/우 레터박스 두께
    float dy = (Ch - fittedH) * 0.5f; // 상/하 레터박스 두께

    // 디버그 보기 좋게 픽셀 스냅(권장)
    dx = std::floor(dx + 0.5f);
    dy = std::floor(dy + 0.5f);
    fittedW = std::floor(fittedW + 0.5f);
    fittedH = std::floor(fittedH + 0.5f);

    // 3) 어느 방향 레터박스인지 결정
    // 대부분 한쪽만 생깁니다. (16:9면 dx=dy=0)
    const float eps = 0.5f; // 반픽셀 정도면 없는 걸로 취급
    bool hasLR = (dx > eps);
    bool hasTB = (dy > eps);

    // 4) 내 역할에 맞게 m_vPos/m_size 세팅
    // "숨김" 시스템이 애매하면 size=0으로 만들어도 됩니다.
    if (!hasLR && !hasTB)
    {
        // 레터박스 없음
        m_size = { 0.0f, 0.0f, 1.0f };
        m_vScale = { 1.0f, 1.0f, 1.0f };
        return true;
    }

    if (hasLR)
    {
        // 좌/우 레터박스: BarA=Left, BarB=Right
        if (m_role == Role::BarA)
        {
            m_vPos = { 0.0f, 0.0f, 0.0f };
            m_size = { dx,   Ch,   1.0f };
        }
        else
        {
            m_vPos = { dx + fittedW, 0.0f, 0.0f };
            m_size = { dx,           Ch,   1.0f };
        }
    }
    else // hasTB
    {
        // 상/하 레터박스: BarA=Top, BarB=Bottom
        if (m_role == Role::BarA)
        {
            m_vPos = { 0.0f, 0.0f, 0.0f };
            m_size = { Cw,   dy,   1.0f };
        }
        else
        {
            m_vPos = { 0.0f, dy + fittedH, 0.0f };
            m_size = { Cw,   dy,          1.0f };
        }
    }

    // 바는 그냥 1배 스케일 유지(크기는 m_size로만 제어)
    m_vScale = { 1.0f, 1.0f, 1.0f };
    return true;
}


bool Letterbox::CreateMesh()
{
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용

    return true;
}

