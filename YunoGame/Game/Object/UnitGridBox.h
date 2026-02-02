#pragma once
#include "Unit.h"

class UnitGridBox : public Unit
{
public:
    explicit UnitGridBox();

    virtual ~UnitGridBox();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
};
