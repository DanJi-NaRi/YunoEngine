#pragma once

#include "Unit.h"

class Building : public Unit
{
public:
    explicit Building();
    virtual ~Building();

    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
};

