#include "pch.h"

#include "Building.h"

#include <IInput.h>
#include "YunoEngine.h"

Building::Building()
{
}

Building::~Building()
{
}

bool Building::Update(float dTime)
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
    }

    Unit::Update(dTime);

    return true;
}

bool Building::Submit(float dTime)
{
    Unit::Submit(dTime);

    return true;
}
