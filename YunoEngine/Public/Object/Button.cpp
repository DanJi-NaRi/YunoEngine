#include "pch.h"
#include "Button.h"

#include "IInput.h"
#include "DragProvider.h"

Button::Button(UIManager* uiManager) : Widget(uiManager)
{
    Clear();
}

Button::~Button()
{
    Clear();
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

    m_btnState = ButtonState::Idle;
    m_bindkey = 0;
    m_anchor = UIDirection::LeftTop;

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

    assert(m_pInput);
    //float mouseX = m_pInput->GetMouseX();
    //float mouseY = m_pInput->GetMouseY();
    //POINT mouseXY{ (LONG)mouseX, (LONG)mouseY };
    POINT mouseXY{ (LONG)m_pInput->GetMouseX(), (LONG)m_pInput->GetMouseY() };

    XMFLOAT3 vPos;
    vPos.x = (float)m_rect.left;
    vPos.y = (float)m_rect.top;
    vPos.z = 1.0f;

    m_btnState = ButtonState::Idle; // 커서 영역 검사 전 기본 상태 초기화.

    if (!PtInRect(&m_rect, mouseXY)) return;	//커서/버튼 영역 검사.

    // 버튼 State 업데이트
    if (m_pInput->IsMouseButtonDown(0)) { m_btnState = ButtonState::Pressed; }
    else if (m_btnState != ButtonState::Pressed && m_pInput->IsMouseButtonPressed(0)) { m_btnState = ButtonState::Pressed; }
    else { m_btnState = ButtonState::Hovered; } // 커서가 올라가있는 것은 확정이므로, 기본값은 Hovered

    switch (m_btnState) // 버튼 상태에 따른 그리기 셋업
    {
    case ButtonState::Idle:	    //버튼 "대기","일반" 출력.
        std::cout << "Idle" << std::endl;
        break;

    case ButtonState::Hovered:	//커서 접촉, "하이라이트" 출력.
        //std::cout << "Hovered" << std::endl;
        break;

    case ButtonState::Pressed:	//"클릭" 상태. "눌림" 출력.
        //std::cout << "Clicked" << std::endl;
        break;

    case ButtonState::Down:	//"클릭" 상태. "눌리고 있음" 출력.
        std::cout << "Down" << std::endl;
        break;
    }
}

bool Button::IdleEvent()
{
    std::cout << "IdleEvent" << std::endl;
    return true;
}

bool Button::HoveredEvent()
{
    std::cout << "HoveredEvent" << std::endl;
    return true;
}
bool Button::DownEvent()
{
    std::cout << "DownEvent" << std::endl;
    return true;
}
bool Button::LMBPressedEvent()
{
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}
bool Button::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

bool Button::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}
bool Button::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}
bool Button::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}
bool Button::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

void Button::Clear() {
    m_pDrag = nullptr;
}
