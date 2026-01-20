#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "UImgui.h"

namespace UI
{
    void SetNextUIPos(UINT x, UINT y)
    {
        ImGui::SetNextWindowPos(ImVec2(x, y));
    }

    void SetNextUISize(UINT width, UINT height)
    {
        ImGui::SetNextWindowSize(ImVec2(width, height));
    }

    void BeginPanel(const char* name)
    {
        ImGui::Begin(name);
    }

    void EndPanel()
    {
        ImGui::End();
    }

    bool Button(const char* label)
    {
        return ImGui::Button(label);
    }

    bool Selectable(const char* label, bool selected)
    {
        return ImGui::Selectable(label, selected);
    }

    void Text(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        ImGui::TextV(fmt, args);
        va_end(args);
    }
}
