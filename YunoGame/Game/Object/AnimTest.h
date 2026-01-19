#pragma once
#include "AnimationUnit.h"

class AnimTest : public AnimationUnit
{
public:
    explicit AnimTest();
    virtual ~AnimTest();

    virtual bool Update(float dTime) override;
};
