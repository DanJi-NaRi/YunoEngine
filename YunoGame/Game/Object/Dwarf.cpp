#include "pch.h"

#include "Dwarf.h"

#include <IInput.h>
#include "YunoEngine.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Dwarf
    {
        AutoReg_Dwarf()
        {
            ObjectTypeRegistry::Instance().Register(L"Dwarf", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Dwarf>(d); });
        }
    } s_reg_Dwarf;
}

Dwarf::Dwarf()
{
    unitType = L"Dwarf";
}

Dwarf::~Dwarf()
{
}


bool Dwarf::Update(float dTime)
{
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
