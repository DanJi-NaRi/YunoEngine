#include "pch.h"

#include "AnimTest.h"

AnimTest::AnimTest()
{
}

AnimTest::~AnimTest()
{
}

bool AnimTest::Update(float dTime)
{
    AnimationUnit::Update(dTime);

    return true;
}
