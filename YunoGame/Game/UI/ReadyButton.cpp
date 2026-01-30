#include "pch.h"
#include "ReadyButton.h"

#include "YunoEngine.h"
#include "IInput.h"


#include "GameManager.h"

#include "PacketBuilder.h"

#include "C2S_ReadySet.h"



ReadyButton::ReadyButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ReadyButton::~ReadyButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ReadyButton::Clear()
{
    m_isReady = false;
}

bool ReadyButton::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool ReadyButton::UpdateTransform(float dTime) {
    Button::UpdateTransform(dTime);

    return true;
}
bool ReadyButton::UpdateLogic(float dTime) {

    return true;
}

bool ReadyButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ReadyButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ReadyButton::HoveredEvent()
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
bool ReadyButton::LMBPressedEvent()
{

    GameManager& gm = GameManager::Get();
    if (gm.GetMyPiece(0)==PieceType::None|| gm.GetMyPiece(1) == PieceType::None) 
    {
        return false;
    }


    const bool isReady = gm.ToggleReady();
    std::wstring texturePath;
    if(!isReady)
        texturePath = L"../Assets/Test/BtnOn.png";
    else
        texturePath = L"../Assets/Test/BtnOff.png";

    m_MeshNode->m_Meshs[0]->SetTexture(TextureUse::Albedo, texturePath);


    // 패킷 초기화
    yuno::net::packets::C2S_ReadySet req{};

    // 패킷에 데이터 담기
    req.readyState = isReady ? 1 : 0;

    // 패킷 바이너리화
    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_ReadySet,
        [&](yuno::net::ByteWriter& w)
        {
            req.Serialize(w);
        });

    // 패킷 보내기
    gm.SendPacket(std::move(bytes));

    return true;
}

// 우클릭 눌렀을 때
bool ReadyButton::RMBPressedEvent()
{
    return true;
}

// 바인딩한 키 눌렀을 때
bool ReadyButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ReadyButton::LMBReleasedEvent()
{
    //std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ReadyButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool ReadyButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

