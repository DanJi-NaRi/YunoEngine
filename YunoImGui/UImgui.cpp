#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

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

    void BeginDisabled(bool b)
    {
        ImGui::BeginDisabled(b);
    }

    void EndDisabled()
    {
        ImGui::EndDisabled();
    }

    void DrawDebugHUD(float* v, float* v2)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("DebugHUD",
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav);

        // ===========================
        // FPS 출력
        // ===========================
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);

        ImGui::Separator();

        // ===========================
        // Camera Position 출력
        // ===========================
        ImGui::Text("Cam Pos:");
        ImGui::Text("(%.2f, %.2f, %.2f)",
        v[0], v[1], v[2]);

        ImGui::Separator();

        ImGui::Text("Cam Target:");
        ImGui::Text("(%.2f, %.2f, %.2f)",
            v2[0], v2[1], v2[2]);

        ImGui::Separator();

        ImGui::End();
    }

    int DrawCameraFovController(float* fovYDeg, float* aspect, float minDeg, float maxDeg)
    {
        if (!fovYDeg || !aspect)
            return 0;

        bool changedY = false;
        bool changedX = false;

        ImGui::SetNextWindowPos(ImVec2(10, 180), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("CameraControl",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoNav);

        ImGui::Text("Camera FOV");
        changedY |= SliderFloat("FOV Y (deg)", fovYDeg, minDeg, maxDeg, "%.1f");
        changedY |= InputFloat("FOV Y Input", fovYDeg, "%.1f");

        changedX |= SliderFloat("Aspect", aspect, 0.001, 3.0f, "%.3f");
        changedX |= InputFloat("Aspect Input", aspect, "%.3f");

        ImGui::End();

        int changedMask = 0;
        if (changedY) changedMask |= 1;
        if (changedX) changedMask |= 2;

        return changedMask;
    }

    bool TreeNodeEx(const void* id, bool selected, bool haschild, const char* name)
    {
        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanFullWidth;

        if(selected)
            flags |= ImGuiTreeNodeFlags_Selected;
        
        if(!haschild)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        return ImGui::TreeNodeEx(id, flags, name);
    }

    void TreePop()
    {
        ImGui::TreePop();
    }

    bool IsItemClicked()
    {
        return ImGui::IsItemClicked();
    }

    bool UI::BeginCombo(const char* label, const char* str)
    {
        return ImGui::BeginCombo(label, str);
    }

    void UI::EndCombo()
    {
        return ImGui::EndCombo();
    }

    void SameLine(float offsetFromX, float space )
    {
        ImGui::SameLine(offsetFromX, space);
    }

    void Separator()
    {
        ImGui::Separator();
    }

    void SetItemDefaultFocus()
    {
        ImGui::SetItemDefaultFocus();
    }

    bool IsItemHovered()
    {
        return ImGui::IsItemHovered();
    }

    bool IsLeftMouseDoubleClicked()
    {
        return ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
    }

    bool Button(const char* label)
    {
        return ImGui::Button(label);
    }

    bool Checkbox(const char* label, bool* v)
    {
        return ImGui::Checkbox(label, v);
    }


    bool Selectable(const char* label, bool selected)
    {
        return ImGui::Selectable(label, selected);
    }

    void NextLine()
    {
        ImGui::Spacing();
    }

    void Text(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        ImGui::TextV(fmt, args);
        va_end(args);
    }

    bool CollapsingHeader(const char* label)
    {
        return ImGui::CollapsingHeader(label,
            ImGuiTreeNodeFlags_DefaultOpen);
    }

    bool InputFloat(const char* label, float* v, const char* format)
    {
        return ImGui::InputFloat(label, v, 0, 0, format);
    }

    bool InputFloat3(const char* label, float* v, const char* format)
    {
        return ImGui::InputFloat3(label, v, format);
    }

    bool IsItemDeactivatedAfterEdit()
    {
        return ImGui::IsItemDeactivatedAfterEdit();
    }

    bool DragFloat3(const char* label, float* v, float speed,
        float v_min, float v_max, const char* format)
    {
        return ImGui::DragFloat3(label, v, speed, v_min, v_max, format);
    }

    bool DragFloat3Editable(
        const char* label,
        float* v,
        float speed,
        float v_min,
        float v_max)
    {
        bool changed = false;
        static bool edit = false;

        std::string displayLabel = label;

        // ## 뒤 제거
        size_t pos = displayLabel.find("##");
        if (pos != std::string::npos)
            displayLabel = displayLabel.substr(0, pos);

        ImGui::PushID(label);

        // 화면에는 displayLabel만 출력
        ImGui::TextUnformatted(displayLabel.c_str());
        ImGui::SameLine();

        const float width = ImGui::CalcItemWidth();
        ImGui::PushMultiItemsWidths(3, width);

        // ===== X (Red) =====
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
        if (!edit)
            changed |= ImGui::DragFloat("##X", &v[0], speed, v_min, v_max);
        else
            changed |= ImGui::InputFloat("##X", &v[0]);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // ===== Y (Green) =====
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(80, 255, 80, 255));
        if (!edit)
            changed |= ImGui::DragFloat("##Y", &v[1], speed, v_min, v_max);
        else
            changed |= ImGui::InputFloat("##Y", &v[1]);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // ===== Z (Blue) =====
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(80, 80, 255, 255));
        if (!edit)
            changed |= ImGui::DragFloat("##Z", &v[2], speed, v_min, v_max);
        else
            changed |= ImGui::InputFloat("##Z", &v[2]);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        // ===== 더블 클릭 → 입력 모드 =====
        if (!edit &&
            ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            edit = true;
        }

        if (edit && ImGui::IsItemDeactivatedAfterEdit())
        {
            edit = false;
        }

        ImGui::PopID();
        return changed;
    }

    bool DragFloat2(const char* label, float* v, float speed,
        float v_min, float v_max, const char* format)
    {
        return ImGui::DragFloat2(label, v, speed, v_min, v_max, format);
    }

    bool DragFloat2Editable(
        const char* label,
        float* v,
        float speed,
        float v_min,
        float v_max)
    {
        bool changed = false;
        static bool edit = false;

        std::string displayLabel = label;

        // ## 뒤 제거
        size_t pos = displayLabel.find("##");
        if (pos != std::string::npos)
            displayLabel = displayLabel.substr(0, pos);

        ImGui::PushID(label);

        // 화면에는 displayLabel만 출력
        ImGui::TextUnformatted(displayLabel.c_str());
        ImGui::SameLine();

        const float width = ImGui::CalcItemWidth();
        ImGui::PushMultiItemsWidths(3, width);

        // ===== X (Red) =====
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
        if (!edit)
            changed |= ImGui::DragFloat("##X", &v[0], speed, v_min, v_max);
        else
            changed |= ImGui::InputFloat("##X", &v[0]);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // ===== Y (Green) =====
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(80, 255, 80, 255));
        if (!edit)
            changed |= ImGui::DragFloat("##Y", &v[1], speed, v_min, v_max);
        else
            changed |= ImGui::InputFloat("##Y", &v[1]);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // ===== 더블 클릭 → 입력 모드 =====
        if (!edit &&
            ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            edit = true;
        }

        if (edit && ImGui::IsItemDeactivatedAfterEdit())
        {
            edit = false;
        }

        ImGui::PopID();
        return changed;
    }

    bool DragFloat(const char* label, float* v, float speed, float v_min, float v_max, const char* format)
    {
        return ImGui::DragFloat(label, v, speed, v_min, v_max, format);
    }

    bool DragFloatEditable(const char* label, float* v, float speed, float v_min, float v_max)
    {
        static bool edit = false;

        if (!edit)
        {
            bool changed = ImGui::DragFloat(label, v, speed, v_min, v_max);

            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                edit = true;
            }

            return changed;
        }
        else
        {
            bool changed = ImGui::InputFloat(label, v);

            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                edit = false;
            }

            return changed;
        }
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt)
    {
        return ImGui::SliderFloat(label, v, v_min, v_max, fmt);
    }

    bool DragInt(const char* label, int* v, int speed, int v_min, int v_max, const char* fmt)
    {
        return ImGui::DragInt(label, v, speed, v_min, v_max, fmt);
    }

    bool DragIntEditable(const char* label, int* v, int speed, int v_min, int v_max)
    {
        static bool edit = false;

        if (!edit)
        {
            bool changed = ImGui::DragInt(label, v, speed, v_min, v_max);

            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                edit = true;
            }

            return changed;
        }
        else
        {
            bool changed = ImGui::InputInt(label, v);

            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                edit = false;
            }

            return changed;
        }
    }
}
