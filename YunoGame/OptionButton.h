#pragma once
#include "Button.h"
#include <functional>

class OptionButton final : public Button
{
public:
    OptionButton(UIFactory& factory)
        : Button(factory)
    {
    }

    void SetHoverTexture(const std::wstring& normal,
        const std::wstring& hover)
    {
        m_normalTex = normal;
        m_hoverTex = hover;
        ChangeTexture(m_normalTex); // 초기 상태
    }

    void SetOnClick(std::function<void()> fn)
    {
        m_onClick = std::move(fn);
    }

    void SetSelected(bool selected)
    {
        m_selected = selected;
        ApplyVisual();
    }

    bool Update(float dt)
    {
        // 1. 먼저 버튼 상태 계산
        Button::ButtonUpdate(dt);

        // 2. 선택 상태면 무조건 hover 고정
        if (m_selected)
        {
            ChangeTexture(m_hoverTex);
            return true;
        }

        // 3. 선택 안 된 경우 버튼 상태에 따라 처리
        switch (m_btnState)
        {
        case ButtonState::Hovered:
        case ButtonState::Pressed:
            ChangeTexture(m_hoverTex);
            break;

        case ButtonState::Idle:
        default:
            ChangeTexture(m_normalTex);
            break;
        }

        return true;
    }
protected:
    bool LMBReleasedEvent() override
    {
        if (m_onClick)
            m_onClick();
        return true;
    }
private:
    void ApplyVisual()
    {
        if (m_selected)
            ChangeTexture(m_hoverTex);   // hover 텍스처 고정
        else
            ChangeTexture(m_normalTex);
    }
private:
    std::function<void()> m_onClick;

    bool m_selected = false;

    std::wstring m_normalTex;
    std::wstring m_hoverTex;
};
