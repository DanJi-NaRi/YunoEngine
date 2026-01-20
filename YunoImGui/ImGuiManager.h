#pragma once

#include <functional>

struct HWND__;
struct ID3D11Device;
struct ID3D11DeviceContext;

class ImGuiManager
{
public:
    static void Initialize(HWND hwnd,
        ID3D11Device* device,
        ID3D11DeviceContext* context);

    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();

    static bool HandleWin32Message(HWND__* hWnd,
        unsigned msg,
        unsigned long long wParam,
        long long lParam);

    static void SetEnabled(bool enable);
    static bool IsEnabled();

    static void RegisterDraw(const std::function<void()>& func);

    static bool ImGuiOwnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


