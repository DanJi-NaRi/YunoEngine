#include "pch.h"
#include "Button.h"

#include "IInput.h"

Button::Button()
{
    m_BtnState = ButtonState::Idle;
}

Button::~Button()
{
}

bool Button::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Widget::Create(name, id, vPos);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    //if (!CreateMesh())
    //    return false;
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용
    if (m_defaultMesh == 0)return false;
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

bool Button::Update(float dTime)
{
    ButtonUpdate(dTime);
    Widget::Update(dTime);

    //std::cout << m_BtnState << std::endl;

    return true;
}

bool Button::Submit(float dTime)
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

void Button::ButtonUpdate(float dTime) // 버튼 상태 갱신
{
    // 영역 검사 및 이벤트 실행은 UIManager에게 위임...
    
    /*
    switch (m_BtnState) {
        case ButtonState::Idle:     IdleEvent();    break;
        case ButtonState::Pressed:  PressedEvent(); break; // 누르고 있는 동안 유지 
        case ButtonState::Down:     DownEvent();    break;            
        case ButtonState::Hovered:  HoveredEvent(); break;  
        default: assert(false && "Invalid ButtonState"); break;
    }
    */
}

bool Button::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    return true;
}
bool Button::HoveredEvent()
{
    //std::cout << "HoveredEvent" << std::endl;
    return true;
}
bool Button::DownEvent()
{
    std::cout << "DownEvent" << std::endl;
    return true;
}
bool Button::PressedEvent()
{
    std::cout << "PressedEvent" << std::endl;
    return true;
}
bool Button::ReleasedEvent()
{
    std::cout << "ReleasedEvent" << std::endl;
    return true;
}


bool Button::CreateMaterial()
{
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/woodbox.bmp");

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

    //m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/Grass.jpg");

    // 추가 머테리얼 생성
    //md.passKey.raster = RasterPreset::CullNone;
    //md.albedo = m_Albedo;

    //m_addMaterial = m_pRenderer->CreateMaterial(md);
    //if (m_addMaterial == 0)
    //    return false;

    return true;
}

