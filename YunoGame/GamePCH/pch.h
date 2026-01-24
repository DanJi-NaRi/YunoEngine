#ifndef PCH_H
#define PCH_H

// ===============================
//           Game PCH
// ===============================

// 1. 여기는 게임용 pch 입니다. 엔진 pch와 별도의 영역입니다.
// 2. 여기서 인클루드 한 헤더들은 YunoGame내에서만 작동합니다.
// 3. 라이브러리만 인클루드 해서 사용할 예정입니다.
// 4. 모든 cpp파일 최상단에 pch.h가 인클루드 되어야 합니다.


#pragma once

// ===============================
// C++ Standard
// ===============================
#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <array>
#include <deque>
#include <queue>
#include <stack>
#include <optional>



// ===============================
// Thread
// ===============================
#include <mutex>
#include <thread>
#include <atomic>


// ===============================
// Debug / Utility
// ===============================
#include <iostream>
#include <sstream>
#include <filesystem>


// ===============================
// DirectX Math
// ===============================
#include <DirectXMath.h>
using namespace DirectX;


// ===============================
// Windows
// ===============================
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>


// ===============================
// YunoMath
// ===============================
#include "YunoTransform.h"



#endif //PCH_H
