#pragma once
#include <cstdint>

enum class InputEventType : uint8_t
{
    KeyDown,
    KeyUp,
    MouseMove,
    MouseHover,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
    MouseLeave, // 커서가 윈도우 밖으로 나감
};

/*
* 김장후 - 추후 추가 고려 (이대로 하란 건 아니고, 참고용)
* 
#define MS_LBTNDRAG	 MS_MOVE|MS_LBTNDOWN	//!< 우측 버튼 드래그.
#define MS_RBTNDRAG	 MS_MOVE|MS_RBTNDOWN	//!< 좌측 버튼 드래그.
#define MS_MBTNDRAG	 MS_MOVE|MS_MBTNDOWN	//!< 중앙 버튼 드래그.
*/

struct InputEvent
{
    InputEventType type{};

    uint32_t key = 0;

    float x = 0.0f;
    float y = 0.0f;

    float delta = 0.0f;

    bool consumed = false;
};
