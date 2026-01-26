#pragma once
#include "Button.h"
#include "CardData.h"

class WeaponButton : public Button
{
public:
    WeaponButton(UIManager* uiManager);
    virtual ~WeaponButton();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    // 버튼 이벤트 핸들
    virtual bool IdleEvent() override;                          // 아무것도 안할 때
    virtual bool HoveredEvent() override;                       // 커서가 위에 있을 때
    virtual bool LMBPressedEvent() override;                    // 왼클릭 눌렀을 때
    virtual bool RMBPressedEvent() override;                    // 우클릭 눌렀을 때
    virtual bool KeyPressedEvent(uint32_t key = 0) override;    // 바인딩한 키 눌렀을 때
    virtual bool LMBReleasedEvent() override;                   // 왼클릭 뗐을 때
    virtual bool RMBReleasedEvent() override;                   // 우클릭 뗐을 때
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;   // 바인딩한 키 뗐을 때

    PieceType GetPieceType() { return m_pieceType; }
    PieceType SetPieceType(PieceType pieceType) { m_pieceType = pieceType; return m_pieceType; }

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::WeaponButton; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)

    void SetUserImages(Widget* U1I1, Widget* U1I2, Widget* U2I1, Widget* U2I2); // 이미지 포인터 받아오기
    void SetSlotIndex();

protected:
    PieceType m_pieceType = PieceType::None;

private:
    std::uint8_t m_mySlotIndex = 0;
    std::uint8_t m_weaponTexId = 0;
    int m_pickIndex = 0;

    Widget* m_pUserImage0 = nullptr; // 왼쪽 1
    Widget* m_pUserImage1 = nullptr; // 왼쪽 2
    Widget* m_pUserImage2 = nullptr; // 오른쪽 1
    Widget* m_pUserImage3 = nullptr; // 오른쪽 2

public:
    void SetMySlotIndex(std::uint8_t idx) { m_mySlotIndex = idx; }
    void SetWeaponTextureId(std::uint32_t texId) { m_weaponTexId = texId; }

    void SetSlotImages(Widget* img0, Widget* img1, Widget* img2, Widget* img3)
    {
        m_pUserImage0 = img0; m_pUserImage1 = img1;
        m_pUserImage2 = img2; m_pUserImage3 = img3;
    }
};
