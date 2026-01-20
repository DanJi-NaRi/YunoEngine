#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <vector>
#include "ImGuiManager.h"

namespace
{
    bool s_initialized = false;
    bool s_enabled = true;

    HWND s_hwnd = nullptr;
    ID3D11Device* s_device = nullptr;
    ID3D11DeviceContext* s_context = nullptr;

    std::vector<std::function<void()>> s_drawList;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ImGuiManager::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (s_initialized)
        return;

    s_hwnd = hwnd;
    s_device = device;
    s_context = context;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(s_hwnd);
    ImGui_ImplDX11_Init(s_device, s_context);

    s_initialized = true;
}

void ImGuiManager::Shutdown()
{
    if (!s_initialized)
        return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    s_drawList.clear();

    s_initialized = false;
}

void ImGuiManager::BeginFrame()
{
    if (!s_initialized || !s_enabled)
        return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    if (!s_initialized || !s_enabled)
        return;

    // 등록된 UI 그리기
    for (auto& draw : s_drawList)
        draw();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiManager::HandleWin32Message(HWND__* hWnd, unsigned msg, unsigned long long wParam, long long lParam)
{
    if (!s_initialized || !s_enabled)
        return false;

    return ImGui_ImplWin32_WndProcHandler(
        hWnd, msg, wParam, lParam);
}

void ImGuiManager::SetEnabled(bool enable)
{
    s_enabled = enable;
}

bool ImGuiManager::IsEnabled()
{
    return s_enabled;
}

void ImGuiManager::RegisterDraw(const std::function<void()>& func)
{
    s_drawList.push_back(func);
}

bool ImGuiManager::ImGuiOwnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

