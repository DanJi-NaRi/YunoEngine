#include "pch.h"

#include "Building.h"

Building::Building()
{
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
