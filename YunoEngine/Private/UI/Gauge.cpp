#include "pch.h"
#include "Gauge.h"
#include "IInput.h"


Gauge::Gauge(UIFactory& uiFactory) : Widget(uiFactory)
{
    m_value = 100.0f;
    m_fillDir = FillDirection::LeftToRight;
}

Gauge::~Gauge()
{
}

bool Gauge::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_gaugeMax = sizePx; // 처음 지정한 사이즈가 게이지의 최대치가 됨.
    return true;

}

bool Gauge::Start()
{
    Widget::Start();

    return true;
}

bool Gauge::Update(float dTime)
{
    Widget::Update(dTime);
    GuageUpdate(dTime);

    return true;
}

bool Gauge::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }

    // 상수 버퍼 추가할당
    m_renderItem.Constant.widgetValueFloat = (m_value / 100);       // value 할당
    m_renderItem.Constant.widgetValueInt = (int)m_fillDir;  // Enum FillDir 할당

    Widget::Submit(dTime);
    return true;
}

void Gauge::GuageUpdate(float dTime)
{

    // value는 0.0f ~ 100.0f
    m_value = std::clamp(m_value, 0.0f, 100.0f);

    // 백분율 계산
    //// m_size = (m_value / 100.0f) * m_gaugeMax;
    //if(IsFillHorizontal()) m_size.x = (m_value / 100.0f) * m_gaugeMax.x;  // 가로 계산
    //else                   m_size.y = (m_value / 100.0f) * m_gaugeMax.y;  // 세로 계산

    // 상수 버퍼 갱신
    m_renderItem.Constant.widgetValueFloat = (m_value / 100);       // value 할당. 미리 나눗셈까지는 해둠
    m_renderItem.Constant.widgetValueInt = (int)m_fillDir;  // Enum FillDir 할당
}

void Gauge::SetFillDirection(FillDirection fillDir)
{
    m_size = m_gaugeMax;
    switch (fillDir) {
    default:
    case FillDirection::LeftToRight: break;
    case FillDirection::RightToLeft: break;
    case FillDirection::TopToBottom: break;
    case FillDirection::BottomToTop: break;
    }
    m_fillDir = fillDir;
    GuageUpdate();
}

bool Gauge::IsFillHorizontal() const
{
    return (m_fillDir == FillDirection::LeftToRight ||
            m_fillDir == FillDirection::RightToLeft);
}


bool Gauge::IsFillVertical() const
{
    return (m_fillDir == FillDirection::TopToBottom || 
            m_fillDir == FillDirection::BottomToTop);
}

bool Gauge::CreateMaterial(std::wstring path, MaterialDesc* pDesc)
{
    m_Albedo = m_pTextures->LoadTexture2D(path.c_str());
    m_texturePath = path;
    m_texturePathBk = path;

    //AddTextureSize(m_Albedo);
    SetTextureSize(m_Albedo);

    MaterialDesc md{};
    if (pDesc) {
        md = *pDesc;
    }
    else {
        md.passKey.vs = ShaderId::UIGauge;
        md.passKey.ps = ShaderId::UIGauge;
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
