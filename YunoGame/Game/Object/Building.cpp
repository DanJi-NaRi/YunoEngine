#include "pch.h"

#include "Building.h"

#include <IInput.h>
#include "YunoEngine.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Building
    {
        AutoReg_Building()
        {
            ObjectTypeRegistry::Instance().Register(L"Building", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Building>(d); });
        }
    } s_reg_Building;
}

Building::Building()
{
    unitType = L"Building";
}

Building::~Building()
{
}


bool Building::Update(float dTime)
{
    Unit::Update(dTime);

    return true;
}

bool Building::Submit(float dTime)
{
    Unit::Submit(dTime);

    return true;
}
