#pragma once
#pragma once

#include "IWindow.h"


class YunoWindow final : public IWindow
{
public:
    YunoWindow();
    ~YunoWindow() override;

    bool Create(const wchar_t* title, uint32_t width, uint32_t height) override;
    void SetClientSize(uint32_t width, uint32_t height) override;
    void PollEvents() override;
    bool ShouldClose() const override { return m_shouldClose; }

    uint32_t GetClientWidth() const override { return m_clientWidth; }
    uint32_t GetClientHeight() const override { return m_clientHeight; }

    void* GetNativeHandle() const override { return m_hWnd; }

    bool ConsumeResize(uint32_t& outW, uint32_t& outH) override;

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void OnResizeFromClientRect();

private:
    HWND     m_hWnd = nullptr;
    bool     m_shouldClose = false;

    uint32_t m_clientWidth = 0; // 현재 해상도
    uint32_t m_clientHeight = 0;// 현재 해상도

private:
    bool     m_resized = false; // 더티 플래그
    uint32_t m_pendingW = 0;    // 바뀐 해상도
    uint32_t m_pendingH = 0;    // 바뀐 해상도
};
