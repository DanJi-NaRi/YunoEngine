#ifndef PCH_H
#define PCH_H

// ===============================
//      NetworkTransport PCH
// ===============================

// 1. 여기는 네트워크 트랜스포트용 pch 입니다. 엔진,게임 pch와 별도의 영역입니다.
// 2. 여기서 인클루드 한 헤더들은 YunoNetTransport내에서만 작동합니다.
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
#include <functional>



#endif //PCH_H
