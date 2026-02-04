#pragma once
#include "Image.h"

class ProgressBar;

class PlayerIcon : public Image
{
public:
    PlayerIcon(UIFactory& uiFactory);
    virtual ~PlayerIcon();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();
    //void LoadDeck(); void DataSend(); // 외부 서비스로 분리
    //void CardDrag();    // 카드 드래그

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardTable; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/Widget/PlayerHUD/Bar_playericon.png"); };    // 머테리얼 생성 (한 번만)
protected:

    std::vector<ProgressBar*> m_bars;
    Image* m_playerImage;
};

