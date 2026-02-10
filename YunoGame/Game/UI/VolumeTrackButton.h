#pragma once
#include "Button.h"
#include "DragProvider.h"

class VolumeTrackButton final : public Button
{
public:
    explicit VolumeTrackButton(UIFactory& factory);

    std::function<void(float)> OnValueChanged;

    bool Update(float dt) override;

    bool Create(
        const std::wstring& name,
        uint32_t id,
        Float2 sizePx,
        XMFLOAT3 vPos,
        float rotZ,
        XMFLOAT3 vScale
    ) override;

    bool LMBPressedEvent() override;
    bool LMBReleasedEvent() override;

    WidgetClass GetWidgetClass() override
    {
        return WidgetClass::VolumeTrackButton;
    }
private:
};
