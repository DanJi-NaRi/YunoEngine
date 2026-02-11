#pragma once
#include "Widget.h"

class TextureImage;
class OptionButton;

class CheckPanel final : public Widget
{
public:
    explicit CheckPanel(UIFactory& factory);

    bool Create(
        const std::wstring& name,
        uint32_t id,
        Float2 sizePx,
        XMFLOAT3 vPos,
        float rotZ,
        XMFLOAT3 vScale
    ) override;

    void CreateChild() override;
    bool Update(float dt) override;

    void SetBackGround(const std::wstring& text);
    void SetYesAction(std::function<void()> fn);
    void SetNoAction(std::function<void()> fn);

    void Show();
    void Hide();

private:
    std::vector<Widget*> m_steps;

    Widget* m_bg;

    OptionButton* m_yesBtn;
    OptionButton* m_noBtn;

    std::function<void()> m_onYes;
    std::function<void()> m_onNo;

    float bgW = 3840.f;
    float bgH = 2160.f;

    float btnW = 279.f /2;
    float btnH = 78.f / 2;
};
