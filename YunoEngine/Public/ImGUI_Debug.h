#pragma once

#include <wrl/client.h>
#include <memory>
#include "UIManager.h"
#include "Widget.h"
#include <unordered_map>
#include "Image.h"
#include "Button.h"
#include "Slot.h"
#include "DragProvider.h"

/////////////////////////////////////////////////////////////////////////////////////
#include <../YunoImGui/core/imgui.h>
static void DrawDebugRect_Client(const RECT& rc)
{
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 base = vp->Pos; // 클라이언트 좌상단(스크린 기준) 오프셋

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddRect(
        ImVec2(base.x + (float)rc.left, base.y + (float)rc.top),
        ImVec2(base.x + (float)rc.right, base.y + (float)rc.bottom),
        IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f
    );
}

static void DrawDebugRect_Client(const RECT& rc, Int3 color)
{
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 base = vp->Pos; // 클라이언트 좌상단(스크린 기준) 오프셋

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddRect(
        ImVec2(base.x + (float)rc.left, base.y + (float)rc.top),
        ImVec2(base.x + (float)rc.right, base.y + (float)rc.bottom),
        IM_COL32(color.x, color.y, color.z, 255), 0.0f, 0, 2.0f
    );
}

/////////////////////////////////////////////////////////////////////////////////////
