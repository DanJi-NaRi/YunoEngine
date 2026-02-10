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
    //int kSteps = 50;
    //
    //float stepW = 16.f;
    //float stepH = 39.f;
    //float gap = 6.f;
    //
    //// 실제 한 칸 이동 거리
    //float pitch = stepW + gap;
    //
    //float totalWidth = (kSteps * stepW) + ((kSteps - 1) * gap);
    //
    //float startX = -totalWidth * 0.5f + stepW * 0.5f;
};
