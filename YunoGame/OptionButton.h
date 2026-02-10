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

    void SetOnClick(std::function<void()> fn)
    {
        m_onClick = std::move(fn);
    }

    bool Update(float dt) override
    {
        Button::Update(dt);
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
    std::function<void()> m_onClick;
};
