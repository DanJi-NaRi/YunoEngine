#pragma once
#include "Image.h"

class ProgressBar;
class Text;

struct IconData
{
    int pId = 0;
    int weaponId = 0;
    int hp = 0;
    int stamina = 0;
};

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
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::PlayerIcon; }

    void SetPlayer(const IconData& idata);
    void SetWeapon(int weaponID);
    void CheckHP();
    void UpdateHPValue(float dTime);
    void CheckStamina();
    void UpdateStaminaValue(float dTime);

    void UpdateIData(const IconData& idata);

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/Bar_playericon.png"); };    // 머테리얼 생성 (한 번만)
protected:
    ProgressBar* m_HpBar;
    ProgressBar* m_StaminaBar;
    Image* m_playerImage;
    IconData m_idata{};
    Text* m_hpText;
    Text* m_StaminaText;

    int maxHp = -1;
    int prevHp = -1;
    int maxStamina = -1;
    int prevStamina = -1;

    float m_curChangeTime = 0.0f;
    float m_changeTime = 0.5f;
    bool isChangingHp = false;
    bool isChangingStamina = false;
};

