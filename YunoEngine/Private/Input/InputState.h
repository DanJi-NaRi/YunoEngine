#pragma once
#include <array>
#include <cstdint>



struct InputState
{

    std::array<uint8_t, 256>    keyDown{};
    std::array<uint8_t, 3>      mouseDown{};    // 좌 = 0, 우 = 1, 가운데 = 2

    std::array<uint8_t, 256>    prevKeyDown{};
    std::array<uint8_t, 3>      prevMouseDown{};

    bool mouseHovered;

    float mouseX = 0.0f;
    float mouseY = 0.0f;

    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;

    // 프레임 시작 시 델타 초기화
    void BeginFrame()
    {
        prevKeyDown = keyDown;
        prevMouseDown = mouseDown;

        mouseDeltaX = 0.0f;
        mouseDeltaY = 0.0f;
    }
};
