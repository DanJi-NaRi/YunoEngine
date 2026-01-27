#pragma once
#include "Unit.h"

class GridBox : public Unit
{
public:
    explicit GridBox();
    virtual ~GridBox();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

};

