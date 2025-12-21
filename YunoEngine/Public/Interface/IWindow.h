#pragma once

#include <cstdint>

class IWindow
{
public:
    virtual ~IWindow() = default;

    // 창 생성
    virtual bool Create(const wchar_t* title, uint32_t width, uint32_t height) = 0;

    // 매 프레임 메시지 처리
    virtual void PollEvents() = 0;

    // 종료 요청 여부
    virtual bool ShouldClose() const = 0;

    // 현재 클라이언트 크기
    virtual uint32_t GetClientWidth() const = 0;
    virtual uint32_t GetClientHeight() const = 0;

    // 렌더러 초기화시 사용 (핸들 받기)
    virtual void* GetNativeHandle() const = 0;

    // 최근 프레임에서 크기 변경이 있었는지
    virtual bool ConsumeResize(uint32_t& outW, uint32_t& outH) = 0;
};