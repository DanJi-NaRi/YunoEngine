#include "pch.h"
#include "ExitButton.h"

#include "YunoEngine.h"
#include "IInput.h"

#include "ISceneManager.h"
#include "IScene.h"
#include "PacketBuilder.h"

#include "C2S_MatchLeave.h"

#include "GameManager.h"


ExitButton::ExitButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ExitButton::~ExitButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ExitButton::Clear()
{
    m_pExitScene = nullptr;
}

bool ExitButton::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool ExitButton::Update(float dTime) {
    Button::Update(dTime);

    return true;
}

bool ExitButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ExitButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ExitButton::HoveredEvent()
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
bool ExitButton::LMBPressedEvent()
{
    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_MatchLeave,
        [&](yuno::net::ByteWriter& w)
        {
            yuno::net::packets::C2S_MatchLeave req{};
            req.Serialize(w);
        });

    GameManager::Get().SendPacket(std::move(bytes));

    GameManager::Get().SetSceneState(CurrentSceneState::Title);

    return true;
}

// 우클릭 눌렀을 때
bool ExitButton::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool ExitButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ExitButton::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ExitButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool ExitButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

void ExitButton::Exit()
{
}

