#include "pch.h"
#include "PopButton.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "AudioQueue.h"

#include "ISceneManager.h"
#include "IScene.h"
#include "PacketBuilder.h"

#include "C2S_MatchLeave.h"

#include "GameManager.h"


PopButton::PopButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

PopButton::~PopButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void PopButton::Clear()
{
    m_pExitScene = nullptr;
}

bool PopButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    sizePx /= 2;
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);
    //SetHoverTexture(m_texturePathBk, L"../Assets/UI/Title/exit_mouseout.png");

    return true;
}

bool PopButton::Update(float dTime) {
    Button::Update(dTime);
    return true;
}

bool PopButton::Submit(float dTime)
{
    Button::Submit(dTime);

    return true;
}

// 가만히 있을 때
bool PopButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성

    return true;
}

bool PopButton::HoverJoinEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseHover));
    return true;
}

// 커서가 위에 올라있을 때
bool PopButton::HoveredEvent()
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
bool PopButton::LMBPressedEvent()
{
    //std::cout << "(LMB)PressedEvent" << std::endl;
    // 게임 끄기
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseClick));

    YunoEngine::GetSceneManager()->RequestPop();


    return true;
}

// 우클릭 눌렀을 때
bool PopButton::RMBPressedEvent()
{
    //std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool PopButton::KeyPressedEvent(uint32_t key)
{
    /*if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;*/
    return true;
}

// 왼클릭 뗐을 때
bool PopButton::LMBReleasedEvent()
{
    //std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool PopButton::RMBReleasedEvent()
{
    //std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool PopButton::KeyReleasedEvent(uint32_t key)
{
    /* if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
     else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;*/
    return true;
}

void PopButton::Exit()
{
}

