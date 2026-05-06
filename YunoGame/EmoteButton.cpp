#include "pch.h"
#include "EmoteButton.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"
#include "GameManager.h"
#include "AudioQueue.h"

//#include "DragProvider.h" // 드래그가 필요하면

EmoteButton::EmoteButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear();
}

EmoteButton::~EmoteButton()
{
    Clear();
}

void EmoteButton::Clear()
{
}

void EmoteButton::SetIdleHoverTextuer(const std::wstring& idlepath, const std::wstring& hoverpath)
{
    SetHoverTexture(idlepath, hoverpath);
}

bool EmoteButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void EmoteButton::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성

}

bool EmoteButton::Start()
{
    Button::Start();
    return true;
}

bool EmoteButton::Update(float dTime) {
    Button::Update(dTime);
    return true;
}

bool EmoteButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool EmoteButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

bool EmoteButton::HoverJoinEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseHover));
    return true;
}

// 커서가 위에 올라있을 때
bool EmoteButton::HoveredEvent()
{
    return true;
}

// 왼클릭 눌렀을 때
bool EmoteButton::LMBPressedEvent()
{
    if (!isLock)
    {
        GameManager::Get().SendEmote(emoteNum);
    }

    return true;
}

// 우클릭 눌렀을 때
bool EmoteButton::RMBPressedEvent()
{
    return true;
}
