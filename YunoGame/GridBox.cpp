#include "pch.h"
#include "GridBox.h"


template class GridBox<Unit>;
template class GridBox<Widget>;

template<typename T>
GridBox<T>::GridBox()
{
}

template<typename T>
GridBox<T>::~GridBox()
{
}

template<typename T>
bool GridBox<T>::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    T::Create(name, id, vPos);
    return true;
}

template<typename T>
bool GridBox<T>::Update(float dTime)
{
    T::Update(dTime);
    return true;
}

template<typename T>
bool GridBox<T>::Submit(float dTime)
{
    T::Submit(dTime);
    return true;
}

//GridBox::GridBox()
//{
//}
//
//GridBox::~GridBox()
//{
//}
//
//bool GridBox::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
//{
//    Unit::Create(name, id, vPos);
//    Backup();
//
//    return true;
//}
//
//bool GridBox::Update(float dTime)
//{
//    Unit::Update(dTime);
//    return true;
//}
//
//bool GridBox::Submit(float dTime)
//{
//    Unit::Submit(dTime);
//    return true;
//}
