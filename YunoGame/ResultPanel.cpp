#include "pch.h"
#include "ResultPanel.h"
#include "YunoEngine.h"
#include "UIFactory.h"
#include "TextureImage.h"

ResultPanel::ResultPanel(UIFactory& factory)
    : Widget(factory)
{
}

bool ResultPanel::Create(
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

void ResultPanel::CreateChild()
{
    // 배경 이미지
    m_bg = m_uiFactory.CreateChild<TextureImage>(
        L"ResultBG",
        sizePx,
        XMFLOAT3(0.f, 0.f, 0.f),
        UIDirection::Center,
        this
    );

    m_bg->SetPivot(UIDirection::Center);

    // 전면 이미지 (연출용)
    m_fg = m_uiFactory.CreateChild<TextureImage>(
        L"ResultFG",
        sizePx,
        XMFLOAT3(0.f, 0.f, 0.f),
        UIDirection::Center,
        this
    );

    m_fg->SetPivot(UIDirection::Center);
}

void ResultPanel::SetImages(const std::wstring& bg, const std::wstring& fg)
{
    if (m_bg)
        m_bg->ChangeTexture(bg);

    if (m_fg)
        m_fg->ChangeTexture(fg);
}

TextureImage* ResultPanel::GetFG()
{
    return m_fg;
}

void ResultPanel::Show()
{
    SetVisible(Visibility::Visible);

    if (m_bg)
        m_bg->SetVisible(Visibility::Visible);

    if (m_fg)
        m_fg->SetVisible(Visibility::Visible);
}

void ResultPanel::Hide()
{
    SetVisible(Visibility::Collapsed);

    if (m_bg)
        m_bg->SetVisible(Visibility::Collapsed);

    if (m_fg)
        m_fg->SetVisible(Visibility::Collapsed);
}

bool ResultPanel::Update(float dt)
{
    return Widget::Update(dt);
}

