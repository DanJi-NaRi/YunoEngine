#include "pch.h"

#include "AnimTest.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_AnimTest
    {
        AutoReg_AnimTest()
        {
            ObjectTypeRegistry::Instance().Register(L"AnimTest", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<AnimTest>(d); });
        }
    } s_reg_AnimTest;
}

AnimTest::AnimTest()
{
    unitType = L"AnimTest";
}

AnimTest::~AnimTest()
{
}

bool AnimTest::Update(float dTime)
{
    AnimationUnit::Update(dTime);

    return true;
}
