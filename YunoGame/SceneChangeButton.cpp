#include "pch.h"
#include "SceneChangeButton.h"
#include "utilityClass.h"

//#include "DragProvider.h" // 드래그가 필요하면
#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"
#include "AudioQueue.h"


// 패킷 관련 인클루드
#include "PacketBuilder.h"
#include "C2S_MatchEnter.h"




SceneChangeButton::SceneChangeButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear();
}

SceneChangeButton::~SceneChangeButton()
{
    Clear();
}

void SceneChangeButton::Clear()
{
    m_targetScene = CurrentSceneState::Title; // 기본값 : Title
}

bool SceneChangeButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    sizePx /= 2;
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void SceneChangeButton::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성
}

bool SceneChangeButton::Start()
{
    Button::Start();
    return true;
}

bool SceneChangeButton::Update(float dTime) {

    if (GameManager::Get().GetSceneState() == CurrentSceneState::CountDown) { m_useHoverEvent = false; return false; }

    Button::Update(dTime);
    return true;
}

bool SceneChangeButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool SceneChangeButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    ChangeTexture(m_texturePathBk);

    return true;
}

// 커서가 위에 올라있을 때
bool SceneChangeButton::HoveredEvent()
{
    //std::cout << "HoveredEvent" << std::endl;

    ChangeTexture(m_hoverdTexturePath);

    //start_mouseover

    return true;
}

bool SceneChangeButton::HoverJoinEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseHover));
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool SceneChangeButton::LMBPressedEvent()
{
    if (GameManager::Get().GetSceneState() == CurrentSceneState::CountDown) { return false; }
    GameManager::Get().SetSceneState(m_targetScene);
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseClick));
    
    return true;
}

// 우클릭 눌렀을 때
bool SceneChangeButton::RMBPressedEvent()
{
    //std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool SceneChangeButton::KeyPressedEvent(uint32_t key)
{
    /*if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;*/
    return true;
}

// 왼클릭 뗐을 때
bool SceneChangeButton::LMBReleasedEvent()
{
   // std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool SceneChangeButton::RMBReleasedEvent()
{
    //std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool SceneChangeButton::KeyReleasedEvent(uint32_t key)
{
   /* if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;*/
    return true;
}
