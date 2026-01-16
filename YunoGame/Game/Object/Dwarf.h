#pragma once

#include "Unit.h"

class Dwarf : public Unit
{
public:
    explicit Dwarf();
    virtual ~Dwarf();

    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
};

