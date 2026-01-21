#include "pch.h"
#include "InputContextBase.h"

bool InputContextBase::Event(InputEvent& evt)
{
    std::cout << "Event" << std::endl;
    return true;
}
