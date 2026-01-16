#include "pch.h"

#include "Dwarf.h"

#include <IInput.h>
#include "YunoEngine.h"

Dwarf::Dwarf()
{
}

Dwarf::~Dwarf()
{
}


bool Dwarf::Update(float dTime)
{
    m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
    m_constant.roughRatio = 1.0f;
    m_constant.roughRatio = 1.0f;
    m_constant.shadowBias = 0.005f;

    IInput* input = YunoEngine::GetInput();
    if (input)
    {
        const float speed = 5.0f;
        const float step = speed * dTime;

        if (input->IsKeyDown(VK_UP)) m_vPos.z += step;
        if (input->IsKeyDown(VK_DOWN)) m_vPos.z -= step;
        if (input->IsKeyDown(VK_LEFT)) m_vPos.x -= step;
        if (input->IsKeyDown(VK_RIGHT)) m_vPos.x += step;

        if (input->IsKeyDown(VK_DELETE)) m_vRot.y -= step / 2;
        if (input->IsKeyDown(VK_END)) m_vRot.y += step / 2;
    }

    Unit::Update(dTime);

    return true;
}

bool Dwarf::Submit(float dTime)
{
    Unit::Submit(dTime);

    return true;
}
