#pragma once
#include "Button.h"


//#include "CardData.h"       // 멤버로 들어야 하면 여기서 Include
//class CardData;
class Slot;                   // 포인터 멤버면 전방선언

class ButtonTemplate : public Button
{
public:
    // 생성자는 (UIFactory& uiFactory) 인자를 기본으로
    ButtonTemplate(UIFactory& uiFactory);
    virtual ~ButtonTemplate();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    void CreateChild() override;         // 위젯 생성 동시에 바로 다음 차례에 자식 위젯을 생성해야 한다면 (자식 생성 공간)
    bool Start() override;                          // 생성 이후, 시작 지점 (자식 객체를 생성한다면 여기에)
    bool Update(float dTime = 0) override;     // UIManager가 전체 순회 업데이트 (Logic)
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

    // 정말 더이상 파생이 없을 것 같으면 final 써주는 것도 추천.
    //virtual bool foo() final override; 

    // 무조건 수정해주기. Class 없으면 WidgetClass 따라가서 추가해주기.
    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Card; }
    
    // 예시 Getter/Setter
    
    // 기본적으로 동사 + 명사
    //PieceMask GetCardData() const { return m_data; }
    //CardData* GetCardData() const { return m_pData; }
    //CardData* GetCardDataPtr() const { return m_pData; } // 멤버를 포인터로 반환할 때 이렇게 하는 것을 추천

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)
protected:
    // 예시 멤버 데이터
    // CardData m_data;                      // 헤더 인클루드
    // CardData* m_pData;                    // 전방선언 + CPP 인클루드, 포인터 타입이라면 m_뒤에 p 붙이기
    // std::unique_ptr<CardData> m_pData;    // 전방선언 + CPP 인클루드

    Slot* m_SetCardSlots = nullptr;
};


// 아래는 주석 제거 버전

/*
#pragma once
#include "Button.h"

class ButtonTemplate : public Button
{
public:
    ButtonTemplate(UIFactory& uiFactory);
    virtual ~ButtonTemplate();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    ButtonTemplate* CreateChild() override;                     // 위젯 생성 동시에 바로 다음 차례에 자식 위젯을 생성해야 한다면 (자식 생성 공간)
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


    virtual WidgetType GetWidgetType() override { return WidgetType::Button; } 
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Card; } // 바꾸기

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)
protected:
};
*/
