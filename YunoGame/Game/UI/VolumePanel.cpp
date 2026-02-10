#include "pch.h"
#include "VolumePanel.h"
#include "YunoEngine.h"
#include "VolumeTrackButton.h"
#include "IInput.h"
#include "UIFactory.h"
#include "TextureImage.h"

VolumePanel::VolumePanel(UIFactory& factory)
    : Widget(factory)
{
}

bool VolumePanel::Create(
    const std::wstring& name,
    uint32_t id,
    Float2 sizePx,
    XMFLOAT3 vPos,
    float rotZ,
    XMFLOAT3 vScale
)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);
    CreateMaterial(L"../Assets/UI/TITLE/volume_unfill.png");
    //CreateChild();
    return true;
}

void VolumePanel::CreateChild()
{
    // === Track (논리 레일, 안 그려짐) ===
    m_track = m_uiFactory.CreateChild<VolumeTrackButton>(
        L"VolumeTrack",
        Float2(totalWidth, stepH/2),
        XMFLOAT3(totalWidth/2, stepH/2, 0),
        UIDirection::Center,
        this
    );

    m_track->OnValueChanged = [this](float t)
        {
            int newLevel = int(std::round(t * (kSteps)));
            newLevel = std::clamp(newLevel, 0, kSteps);

            std::cout << "[Panel] Level = " << newLevel << "\n";

            SetLevel(newLevel);
        };

    //// === Steps ===
    //TODO :: 위치 확인
    for (int i = 0; i < kSteps; ++i)
    {
        float x = startX + pitch * i;
        x = std::round(x);
    
        auto* step = m_uiFactory.CreateChild<TextureImage>(
            L"VolumeStep",
            Float2(stepW, stepH),
            XMFLOAT3(x, 0.f, 0.f),
            UIDirection::LeftTop,
            this
        );
    
        step->ChangeTexture(L"../Assets/UI/TITLE/volume_unfill.png");
        m_steps.push_back(step);
    }
    
    SetLevel(m_level);
}

bool VolumePanel::Update(float dt)
{
    return Widget::Update(dt);
}

void VolumePanel::SetLevel(int level)
{
    m_level = std::clamp(level, 0, kSteps);

    std::cout << "[SetLevel] apply = " << m_level << "\n";

    m_steps[1]->ChangeTexture(L"../Assets/UI/TITLE/volume_fill.png");

    for (int i = 0; i < kSteps; ++i)
    {
        m_steps[i]->ChangeTexture(
            i < m_level
            ? L"../Assets/UI/TITLE/volume_fill.png"
            : L"../Assets/UI/TITLE/volume_unfill.png"
        );
    }
}
