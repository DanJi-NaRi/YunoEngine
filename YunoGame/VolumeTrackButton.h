#pragma once
#include "Button.h"
#include "DragProvider.h"

class VolumeTrackButton final : public Button
{
public:
    VolumeTrackButton(UIFactory& factory)
        : Button(factory)
    {
    }

    bool Create(
        const std::wstring& name,
        uint32_t id,
        Float2 sizePx,
        XMFLOAT3 vPos,
        float rotZ,
        XMFLOAT3 vScale
    ) override
    {
        Button::Create(name, id, sizePx, vPos, rotZ, vScale);

        m_pDrag = std::make_unique<DragProvider>();
        m_pDrag->Init(&m_vPos, true, &m_uiFactory, this);

        return true;
    }
};
