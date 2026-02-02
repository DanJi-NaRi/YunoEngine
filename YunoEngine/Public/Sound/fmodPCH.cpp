#pragma once
#include "pch.h"
#include "fmodPCH.h"

void CheckFMOD(FMOD_RESULT r, const char* where)
{
    if (r != FMOD_OK)
    {
        std::cerr << "[FMOD ERROR] " << where << " : " << r << "\n";
    }
}
