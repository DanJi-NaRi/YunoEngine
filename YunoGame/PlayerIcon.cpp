#include "pch.h"
#include "PlayerIcon.h"

#include "HealthBar.h"
#include "StaminaBar.h"

//#include "UnitPiece.h"

#include "IInput.h"
#include "UIFactory.h"
#include "Gauge.h"
#include "Text.h"

int Lerp(int prev, int Cur, float a)
{
    return prev * (1 - a) + Cur * a;
}

PlayerIcon::PlayerIcon(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

PlayerIcon::~PlayerIcon()
{
    Clear();
}

void PlayerIcon::Clear()
{
    m_HpBar = nullptr;
    m_StaminaBar = nullptr;

    maxHp = -1;
    prevHp = -1;
    maxStamina = -1;
    prevStamina = -1;

    m_idata = {};
}

void PlayerIcon::SetPlayer(const IconData& idata)
{
    if(m_HpBar)
        m_HpBar->SetFillDirect(FillDirection::RightToLeft);
    if(m_StaminaBar)
        m_StaminaBar->SetFillDirect(FillDirection::RightToLeft);

    m_idata = idata;

    SetWeapon(m_idata.weaponId);
    CheckHP();
    CheckStamina();

    maxHp = m_idata.hp;
    maxStamina = m_idata.stamina;

    prevHp = maxHp;
    prevStamina = maxStamina;

    m_hpText->SetText(std::to_wstring(prevHp) + L" / " + std::to_wstring(maxHp));
    m_StaminaText->SetText(std::to_wstring(prevStamina) + L" / " + std::to_wstring(maxStamina));
}

void PlayerIcon::SetWeapon(int weaponID)
{
    if (m_idata.pId < 0) return;

    switch (weaponID)
    {
    case 1:
        if(m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_blaster.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_blaster.png");
        break;
    case 2:
        if (m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_chakram.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_chakram.png");
        break;
    case 3:
        if (m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_breacher.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_breacher.png");
        break;
    case 4:
        if (m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_scythe.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_scythe.png");
        break;
    case 5:
        if (m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_impactor.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_impactor.png");
        break;
    case 6:
        if (m_idata.pId == 1)
            ChangeTexture(L"../Assets/UI/PLAY/profile_1p_cleaver.png");
        else
            ChangeTexture(L"../Assets/UI/PLAY/profile_2p_cleaver.png");
        break;
    }
}

void PlayerIcon::CheckHP()
{
    if (m_idata.hp == prevHp || isChangingHp)
        return;

    isChangingHp = true;
}

void PlayerIcon::UpdateHPValue(float dTime)
{
    if (!isChangingHp) return;

    m_curChangeTime += dTime;

    float t = m_curChangeTime / m_changeTime;
    t = std::clamp(t, 0.0f, 1.0f);

    float curHp = prevHp + (m_idata.hp - prevHp) * t;

    float percent = curHp / maxHp;
    m_HpBar->SetGaugeValue((int)(percent * 100.0f));
    m_hpText->SetText(std::to_wstring(static_cast<int>(curHp)) + L" / " + std::to_wstring(maxHp));

    if (t >= 1.0f)
    {
        m_curChangeTime = 0.0f;
        isChangingHp = false;
        prevHp = m_idata.hp;
    }
}

void PlayerIcon::CheckStamina()
{
    if (m_idata.stamina == prevStamina || isChangingStamina)
        return;

    isChangingStamina = true;
}

void PlayerIcon::UpdateStaminaValue(float dTime)
{
    if (!isChangingStamina) return;

    m_curChangeTime += dTime;

    float t = m_curChangeTime / m_changeTime;
    t = std::clamp(t, 0.0f, 1.0f);

    float curStamina = prevStamina + (m_idata.stamina - prevStamina) * t;

    float percent = curStamina / maxStamina;
    m_StaminaBar->SetGaugeValue((int)(percent * 100.0f));
    m_StaminaText->SetText(std::to_wstring(static_cast<int>(curStamina)) + L" / " + std::to_wstring(maxStamina));

    if (t >= 1.0f)
    {
        m_curChangeTime = 0.0f;
        isChangingStamina = false;
        prevStamina = m_idata.stamina;
    }
}

void PlayerIcon::UpdateIData(const IconData& idata)
{
    if (m_idata.pId != idata.pId || m_idata.weaponId != idata.weaponId)
    {
        std::cout << "pID, weapon ID가 초기화 시점과 다름" << std::endl;
    }

    m_idata = idata;

    CheckHP();
    CheckStamina();
}

bool PlayerIcon::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용
    if (m_defaultMesh == 0)return false;
    if (!CreateMaterial())
        return false;

    m_MeshNode = std::make_unique<MeshNode>();

    auto mesh = std::make_unique<Mesh>();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    m_MeshNode->m_Meshs.push_back(std::move(mesh));

    {
        m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
        m_constant.roughRatio = 1.0f;
        m_constant.metalRatio = 1.0f;
        m_constant.shadowBias = 0.005f;
    }

    m_anchor = UIDirection::LeftTop;

    this->SetLayer(WidgetLayer::Panels);

    Backup();

    return true;
}


void PlayerIcon::CreateChild() {
    m_HpBar = m_uiFactory.CreateChild<HealthBar>(m_name + L"_HealthBar", Float2(542, 17), XMFLOAT3(300, -24, 0), UIDirection::Center, WidgetLayer::HUD, this);
    m_StaminaBar = m_uiFactory.CreateChild<StaminaBar>(m_name + L"_StaminaBar", Float2(542, 17), XMFLOAT3(310, 0, 0), UIDirection::Center, WidgetLayer::HUD, this);
    /*m_hpText = m_uiFactory.CreateChild<Text>(m_name + L"_HpText", Float2(200, 50), XMFLOAT3(0, 0, 0), UIDirection::Center, WidgetLayer::HUD, this);
    m_hpText->SetFont(FontID::Number);
    m_StaminaText = m_uiFactory.CreateChild<Text>(m_name + L"_StaminaText", Float2(200, 50), XMFLOAT3(0, 0, 0), UIDirection::Center, WidgetLayer::HUD, this);
    m_StaminaText->SetFont(FontID::Number);*/
}

bool PlayerIcon::Start()
{
    Image::Start();

    return true;
}

bool PlayerIcon::Update(float dTime)
{
    Image::Update(dTime);

    UpdateHPValue(dTime);
    UpdateStaminaValue(dTime);

    return true;
}

bool PlayerIcon::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

