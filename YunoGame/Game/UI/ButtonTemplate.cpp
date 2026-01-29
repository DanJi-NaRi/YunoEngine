#include "pch.h"
#include "ButtonTemplate.h"
#include "DragProvider.h" // 드래그가 필요하면

#include "YunoEngine.h"
#include "IInput.h"

ButtonTemplate::ButtonTemplate(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ButtonTemplate::~ButtonTemplate()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ButtonTemplate::Clear() 
{
    // 기본적으로 Clear()함수를 만들어 쓰는 게 패턴.
    // 생성자 및 소멸자에서 재활용하는 용도다. 즉, 보통은 nullptr 용도.
    // 특정한 곳에서만 사용한다면, 그 곳에 따로 로직을 작성하도록 하자.

    // m_pData = nullptr;
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    // 피벗 설정 2가지 // 기본값 : LeftTop // 1, 2번을 추천한다.
    // DragProvider를 통해 스냅을 할 경우, Slot과 피벗을 맞추도록 하자.
    //방법 1 : SetPivot(Float2{1,1});                               // 세부적으로 피벗 설정하고 싶으면
    //방법 2 : SetPivot(UIDirection::Center);                       // 고정값으로 피벗 설정하고 싶으면
    //방법 3 : m_pivot = PivotFromUIDirection(UIDirection::Center); // 전역함수(비추천)
   

    // 키 바인드가 필요하면 할당할 수 있음. 
    // 알파벳은 'A'와 같은 형태로도 할당 가능.
    // 아닐 경우, 여타 다른 키처럼 등록
    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    //m_pDrag = std::make_unique<DragProvider>();  // 드래그 기능 사용 시 추가
    //if (!m_pDrag) return false;                  // 드래그 기능 사용 시 추가

    m_pDrag->Init(m_pInput, &m_vPos, true);
    return true;
}

bool ButtonTemplate::Update(float dTime)
{
    //m_pDrag->UpdateDrag(dTime);                   // 드래그 기능 사용 시 추가
    Button::Update(dTime);
    return true;
}

bool ButtonTemplate::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ButtonTemplate::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ButtonTemplate::HoveredEvent()
{
    std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool ButtonTemplate::LMBPressedEvent()
{
    //m_pDrag->StartDrag();                         //  드래그 기능 사용 시 추가 (왼클릭임 주의)
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool ButtonTemplate::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool ButtonTemplate::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ButtonTemplate::LMBReleasedEvent()
{
    //m_pDrag->EndDrag();                           //  드래그 기능 사용 시 추가 (왼클릭임 주의)
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ButtonTemplate::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때 
bool ButtonTemplate::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

// 아래는 주석 제거 버전

/*

 #include "DragProvider.h" // 드래그가 필요하면

#include "YunoEngine.h"
#include "IInput.h"

ButtonTemplate::ButtonTemplate(UIFactory& uiFactory) : Button(uiManager) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ButtonTemplate::~ButtonTemplate()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ButtonTemplate::Clear()
{
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    //m_pDrag = std::make_unique<DragProvider>();  // 드래그 기능 사용 시 추가
    //if (!m_pDrag) return false;                  // 드래그 기능 사용 시 추가

    m_pDrag->Init(m_pInput, &m_vPos, true);
    return true;
}

bool ButtonTemplate::Update(float dTime)
{
    //m_pDrag->UpdateDrag(dTime);                   // 드래그 기능 사용 시 추가
    Button::Update(dTime);
    return true;
}

bool ButtonTemplate::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ButtonTemplate::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ButtonTemplate::HoveredEvent()
{
    std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool ButtonTemplate::LMBPressedEvent()
{
    //m_pDrag->StartDrag();                         //  드래그 기능 사용 시 추가 (왼클릭임 주의)
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool ButtonTemplate::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool ButtonTemplate::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ButtonTemplate::LMBReleasedEvent()
{
    //m_pDrag->EndDrag();                           //  드래그 기능 사용 시 추가 (왼클릭임 주의)
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ButtonTemplate::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool ButtonTemplate::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

*/

// 아래는 주석 + 드래그 제거 버전


/*
 
#include "YunoEngine.h"
#include "IInput.h"

ButtonTemplate::ButtonTemplate(UIFactory& uiFactory) : Button(uiManager) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ButtonTemplate::~ButtonTemplate()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ButtonTemplate::Clear()
{
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool ButtonTemplate::Update(float dTime)
{
    Button::Update(dTime);
    return true;
}

bool ButtonTemplate::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ButtonTemplate::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ButtonTemplate::HoveredEvent()
{
    std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool ButtonTemplate::LMBPressedEvent()
{
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool ButtonTemplate::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool ButtonTemplate::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ButtonTemplate::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ButtonTemplate::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool ButtonTemplate::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

*/
