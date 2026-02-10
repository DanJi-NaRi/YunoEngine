#include "pch.h"
#include "VolumeTrackButton.h"
#include "YunoEngine.h"
#include "IInput.h"  
VolumeTrackButton::VolumeTrackButton(UIFactory& factory)
    : Button(factory)
{
}

bool VolumeTrackButton::Update(float dt)
{//TODO :: 위치 확인
    Button::Update(dt);

    if (!m_pDrag || !m_pDrag->IsNowDragging())
        return true;

    RECT rc = GetRect();

    int mouseX = YunoEngine::GetInput()->GetMouseX();

    float t = float(mouseX - rc.left) / float(rc.right - rc.left);
    t = std::clamp(t, 0.f, 1.f);

    std::cout << "[Track] t = " << t << "\n";

    if (OnValueChanged)
        OnValueChanged(t);

    return true;
}

bool VolumeTrackButton::Create(
    const std::wstring& name,
    uint32_t id,
    Float2 sizePx,
    XMFLOAT3 vPos,
    float rotZ,
    XMFLOAT3 vScale
)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    ChangeTexture(L"../Assets/UI/TITLE/volume_fill_full.png");

    m_pDrag = std::make_unique<DragProvider>();
    m_pDrag->Init(&m_vPos, true, &m_uiFactory, this);

    return true;
}

bool VolumeTrackButton::LMBPressedEvent()
{
    std::cout << "[Track] Drag START\n";
    if (m_pDrag) m_pDrag->StartDrag();
    return true;
}

bool VolumeTrackButton::LMBReleasedEvent()
{
    std::cout << "[Track] Drag END\n";
    if (m_pDrag) m_pDrag->EndDrag();
    return true;
}
