#include "pch.h"
#include "CheckPanel.h"
#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"
#include "TextureImage.h"
#include "OptionButton.h"

CheckPanel::CheckPanel(UIFactory& factory)
    : Widget(factory)
{
}

bool CheckPanel::Create(
    const std::wstring& name,
    uint32_t id,
    Float2 sizePx,
    XMFLOAT3 vPos,
    float rotZ,
    XMFLOAT3 vScale
)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);
    //CreateMaterial(L"../Assets/UI/TITLE/volume_unfill.png");
    return true;
}

void CheckPanel::CreateChild()
{
    // 배경
    m_bg = m_uiFactory.CreateChild<TextureImage>(L"BackGround",
        Float2(bgW, bgH),
        XMFLOAT3(0.f, 0.f, 0.f),
        UIDirection::Center,
        this
    );

    // Yes
    m_yesBtn = m_uiFactory.CreateChild<OptionButton>(L"YesBtn",
        Float2(btnW, btnH),
        XMFLOAT3(-150.f, 20.f, 0.f),
        UIDirection::LeftTop,
        this
    );

    m_yesBtn->SetHoverTexture(
        L"../Assets/UI/PLAY/yes_mouseout.png",
        L"../Assets/UI/PLAY/yes_mouseover.png"
    );

    m_yesBtn->SetOnClick([this]()
        {
            if (m_onYes)
                m_onYes();
        });

    // No
    m_noBtn = m_uiFactory.CreateChild<OptionButton>(L"NoBtn",
        Float2(btnW, btnH),
        XMFLOAT3(20.f, 20.f, 0.f),
        UIDirection::LeftTop,
        this
    );

    m_noBtn->SetHoverTexture(
        L"../Assets/UI/PLAY/no_mouseout.png",
        L"../Assets/UI/PLAY/no_mouseover.png"
    );

    m_noBtn->SetOnClick([this]()
        {
            if (m_onNo)
                m_onNo();
        });
}

void CheckPanel::SetBackGround(const std::wstring& texPath)
{
    if (!m_bg)
        return;

    auto* img = dynamic_cast<TextureImage*>(m_bg);
    if (img)
        img->ChangeTexture(texPath);
}

void CheckPanel::SetYesAction(std::function<void()> fn)
{
    m_onYes = std::move(fn);
}

void CheckPanel::SetNoAction(std::function<void()> fn)
{
    m_onNo = std::move(fn);
}

void CheckPanel::Show()
{
    SetVisible(Visibility::Visible);

    if (m_bg)
        m_bg->SetVisible(Visibility::Visible);

    if (m_yesBtn)
        m_yesBtn->SetVisible(Visibility::Visible);

    if (m_noBtn)
        m_noBtn->SetVisible(Visibility::Visible);
}

void CheckPanel::Hide()
{
    SetVisible(Visibility::Collapsed);

    if (m_bg)
        m_bg->SetVisible(Visibility::Collapsed);

    if (m_yesBtn)
        m_yesBtn->SetVisible(Visibility::Collapsed);

    if (m_noBtn)
        m_noBtn->SetVisible(Visibility::Collapsed);
}

bool CheckPanel::Update(float dt)
{
    return Widget::Update(dt);
}

