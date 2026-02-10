#pragma once
#include "Widget.h"

class VolumeTrackButton;

class VolumePanel final : public Widget
{
public:
    explicit VolumePanel(UIFactory& factory);

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

    int  GetLevel() const { return m_level; }
    void SetLevel(int level);

private:
    VolumeTrackButton* m_track = nullptr;
    std::vector<Widget*> m_steps;
    //TODO :: 위치 확인
    int m_level = 10;

    int kSteps = 50;

    float stepW = 16.f;
    float stepH = 39.f;
    float gap = 5.f;

    // 실제 한 칸 이동 거리
    float pitch = stepW - gap;

    float totalWidth = (kSteps * stepW) + ((kSteps - 1) * gap);

    float startX = -totalWidth * 0.5f + stepW * 0.5f;

};
