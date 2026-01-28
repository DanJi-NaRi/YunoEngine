#include "pch.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_Player //<- 이름은 자유
    {
        AutoReg_Player()
        {
            //아래줄 주석 풀고 이름만 바꿔주고 등록하면됩니다
            //ObjectTypeRegistry::Instance().Register(L"Player", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Player>(d); });
        }
    } s_reg_player;//<- 이름은 자유
}
