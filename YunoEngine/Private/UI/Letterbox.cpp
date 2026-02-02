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

    m_useAspectComp = false; // 화면비 스케일 끄기

    return true;
}


bool Letterbox::Update(float dTime)
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

