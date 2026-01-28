#include "pch.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Player //<- 이름은 자유
    {
        AutoReg_Player()
        {
            ObjectTypeRegistry::Instance().Register(L"Player", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Player>(d); });
        }
    } s_reg_player;//<- 이름은 자유
}
