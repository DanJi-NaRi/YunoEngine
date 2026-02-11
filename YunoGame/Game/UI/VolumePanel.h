#pragma once
#include "Widget.h"

class VolumeTrackButton;

enum class VolumeType
{
    Master,
    BGM,
    SFX
};

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

    void SetVolumeType(int typeIndex){ m_type = static_cast<VolumeType>(typeIndex); }
private:
    VolumeTrackButton* m_track = nullptr;
    std::vector<Widget*> m_steps;

    VolumeType m_type = VolumeType::Master;

    int m_level = 10;

    int kSteps = 50;

    float stepW = 16.f;
    float stepH = 39.f;
    float gap = 3.f;

    // 실제 한 칸 이동 거리
    float pitch = stepW/2 + gap;

    float totalWidth = (kSteps * pitch) - gap;     // 1칸 + 우측 공배 = pitch

    float startX = -totalWidth * 0.5f ;

};
