#pragma once

#include <fmod_studio.hpp>
#include <fmod.hpp>

#ifdef _DEBUG
#pragma comment(lib, "fmodstudioL_vc.lib")
#pragma comment(lib, "fmodL_vc.lib")
#else
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "fmod_vc.lib")
#endif

void CheckFMOD(FMOD_RESULT r, const char* where);
