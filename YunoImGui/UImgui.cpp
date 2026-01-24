#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>

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
        return ImGui::DragFloat3(label, v, 0.1f, v_min, v_max, format);
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

        ImGui::PushID(label);

        ImGui::TextUnformatted(label);
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

    bool DragFloat(const char* label, float* v, float speed, float v_min, float v_max, const char* format)
    {
        return ImGui::DragFloat(label, v, 0.1f, v_min, v_max, format);
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
}
