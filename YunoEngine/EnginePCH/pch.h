#ifndef PCH_H
#define PCH_H

// ===============================
//           Engine PCH
// ===============================

// 1. 여기는 엔진용 pch 입니다. 게임 pch와 별도의 영역입니다.
// 2. 여기서 인클루드 한 헤더들은 YunoEngine 내에서만 작동합니다.
// 3. 라이브러리만 인클루드 해서 사용할 예정입니다.
// 4. 모든 cpp파일 최상단에 pch.h가 인클루드 되어야 합니다.


#pragma once


// ===============================
// STL
// ===============================


#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <chrono>
#include <thread>



// ===============================
// Win32
// ===============================
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



// ===============================
// DirectX 11
// ===============================
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>



// ===============================
// DirectX Math
// ===============================
#include <DirectXMath.h>
using namespace DirectX;



// ===============================
// WRL / COM
// ===============================
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;



// ===============================
// Debug / Utility
// ===============================
#include <iostream>
#include <sstream>
#include <filesystem>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif



#endif //PCH_H
