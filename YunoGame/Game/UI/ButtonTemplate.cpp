#include "pch.h"
#include "ButtonTemplate.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"

#include "DragProvider.h" // 드래그가 필요하면

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
    m_SetCardSlots = nullptr;
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    // 피벗 설정 2가지 // 기본값 : LeftTop // 1, 2번을 추천한다.
    // DragProvider를 통해 스냅을 할 경우, Slot과 피벗을 맞추도록 하자.
    // 피벗은 생성하면서 지정하므로, 굳이 설정해주지 않아도 된다.
    //방법 1 : SetPivot(Float2{1,1});                               // 세부적으로 피벗 설정하고 싶으면
    //방법 2 : SetPivot(UIDirection::Center);                       // 고정값으로 피벗 설정하고 싶으면
    //방법 3 : m_pivot = PivotFromUIDirection(UIDirection::Center); // 전역함수(비추천)
   
    // 레이어 설정 2가지
    // 레이어는 생성하면서 지정하지 않으므로, 별개로 해주어야 한다.
    // Enum에 대응하는 int가 낮을수록, 뒤에 위치하게 된다.
    // UI의 정렬 우선순위는, 1:Layer 2:부모자식 이다.
    //방법 1 : m_layer = WidgetLayer::Default;
    //방법 2 : SetLayer(WidgetLayer::Default);

    // 키 바인드가 필요하면 할당할 수 있음. 
    // 알파벳은 'A'와 같은 형태로도 할당 가능.
    // 아닐 경우, 여타 다른 키처럼 등록
    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    //m_pDrag = std::make_unique<DragProvider>();  // 드래그 기능 사용 시 추가
    //if (!m_pDrag) return false;                  // 드래그 기능 사용 시 추가

    // 캔버스/클라이언트 크기에 스케일/Pos 영향을 받고 싶지 않다면 false 해주기. (UpdateTransform에 영향)
    // 기본값은 True. // 만약 부모가 있다면, false로 취급.
    //m_useAspectComp = false;

    m_pDrag->Init(&m_vPos, true, &m_uiFactory);

    return true;
}

void ButtonTemplate::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성

    // 다양한 사용법 예시
    //m_uiFactory.CreateChild<Widget>(m_name + L"_S3", Float2(50, 85), XMFLOAT3(390, -240, 0), UIDirection::LeftTop, WidgetLayer::Default, this);
    //m_setCardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S3", Float2(50, 85), XMFLOAT3(390, -240, 0), UIDirection::LeftTop, this));
    //m_cardConfirmButton = m_uiFactory.CreateChild<CardConfirmButton>(m_name + L"_CardConfirmButton", Float2(200, 50), XMFLOAT3(100, -100, 0), UIDirection::LeftTop, this);
}

bool ButtonTemplate::Start()
{
    Button::Start();
    return true;
}

bool ButtonTemplate::Update(float dTime) {
    Button::Update(dTime);
    //m_pDrag->UpdateDrag(dTime);                   // 드래그 기능 사용 시 추가
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
    //std::cout << "HoveredEvent" << std::endl;
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

#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"

//#include "DragProvider.h" // 드래그가 필요하면

ButtonTemplate::ButtonTemplate(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear();
}

ButtonTemplate::~ButtonTemplate()
{
    Clear();
}

void ButtonTemplate::Clear()
{
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void ButtonTemplate::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성

}

bool ButtonTemplate::Start()
{
    Button::Start();
    return true;
}

bool ButtonTemplate::Update(float dTime) {
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



// 아래는 드래그 + 주석 제거 버전

/*

#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"

#include "DragProvider.h" // 드래그가 필요하면

ButtonTemplate::ButtonTemplate(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear();
}

ButtonTemplate::~ButtonTemplate()
{
    Clear();
}

void ButtonTemplate::Clear()
{
}

bool ButtonTemplate::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    m_pDrag = std::make_unique<DragProvider>();
    if (!m_pDrag) return false;                

    m_pDrag->Init(m_pInput, &m_vPos, true);
    return true;
}

void ButtonTemplate::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성

}

bool ButtonTemplate::Start()
{
    Button::Start();
    return true;
}

bool ButtonTemplate::Update(float dTime) {
    Button::Update(dTime);
    m_pDrag->UpdateDrag(dTime);                   
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
    m_pDrag->StartDrag();                         //  드래그 기능 사용 시 추가 (왼클릭임 주의)
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
    m_pDrag->EndDrag();                           //  드래그 기능 사용 시 추가 (왼클릭임 주의)
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
