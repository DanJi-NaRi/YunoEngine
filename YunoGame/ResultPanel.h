#pragma once
#include "Widget.h"

class TextureImage;

class ResultPanel final : public Widget
{
public:
    explicit ResultPanel(UIFactory& factory);

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

    void SetImages(
        const std::wstring& bg,
        const std::wstring& fg
    );

    TextureImage* GetFG();// 연출?

    void Show();
    void Hide();

private:
    TextureImage* m_bg;
    TextureImage* m_fg;

    Float2 sizePx = {3840.f, 2160.f};

};
