#pragma once
#include "Unit.h"
#include "Widget.h"

template<typename T>
class GridBox : public T
{
public:
    explicit GridBox();
    template<typename ...Args>
    GridBox(Args && ...args) : T(std::forward<Args>(args)...)
    {
    }

    virtual ~GridBox();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
};
