#pragma once
#include <cstdint>

enum class InputEventType : uint8_t
{
    KeyDown,
    KeyUp,
    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel
};

struct InputEvent
{
    InputEventType type{};

    uint32_t key = 0;

    float x = 0.0f;
    float y = 0.0f;

    float delta = 0.0f;

    bool consumed = false;
};
