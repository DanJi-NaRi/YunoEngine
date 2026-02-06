#pragma once
#include "Image.h"
#include "WidgetGridSystem.h"

class WidgetGridSystem;

class Minimap : public Image
{
public:
    Minimap(UIFactory& uiFactory);
    virtual ~Minimap();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();
    //void LoadDeck(); void DataSend(); // 외부 서비스로 분리
    //void CardDrag();    // 카드 드래그

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Minimap; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/window_popup_battlesetup.png"); };    // 머테리얼 생성 (한 번만)
protected:
    // 세팅된 카드 / 스냅 위치 정보

   WidgetGridSystem& m_map;           // 미니맵 // 스폰 포지션 따로 받기?
};

