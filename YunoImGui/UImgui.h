#pragma once

namespace UI
{
    void SetNextUIPos(UINT x, UINT y);
    void SetNextUISize(UINT width, UINT height);

    void BeginPanel(const char* name);
    void EndPanel();

    void BeginDisabled(bool b = true);
    void EndDisabled();

    void DrawDebugHUD(float* v, float* v2);

    bool TreeNodeEx(const void* id, bool selected, bool haschild, const char* name);
    void TreePop();

    bool IsItemClicked();

    bool BeginCombo(const char* label, const char* str);
    void EndCombo();

    void SameLine(float offsetFromX = 0.0f, float space = -1.0f);

    void Separator();

    void SetItemDefaultFocus();

    bool IsItemHovered();
    bool IsLeftMouseDoubleClicked();

    bool Button(const char* label);
    void Text(const char* fmt, ...);

    bool CollapsingHeader(const char* label);

    bool InputFloat(const char* label, float* v, const char* format = "%.1f");
    bool InputFloat3(const char* label, float* v, const char* format = "%.1f");
    bool IsItemDeactivatedAfterEdit();

    bool DragFloat3(const char* label, float* v, float speed = 0.1f/*움직이는 픽셀당 단위*/,
        float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.1f");
    bool DragFloat3Editable(const char* label, float* v, float speed, float v_min = 0.0f, float v_max = 0.0f);

    bool DragFloat2(const char* label, float* v, float speed = 0.1f/*움직이는 픽셀당 단위*/,
        float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.1f");
    bool DragFloat2Editable(const char* label, float* v, float speed, float v_min = 0.0f, float v_max = 0.0f);

    bool DragFloat(const char* label, float* v, float speed = 0.1f/*움직이는 픽셀당 단위*/,
        float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.1f");
    bool DragFloatEditable(const char* label, float* v, float speed, float v_min, float v_max);
    
    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt = "%.1f");

    bool DragInt(const char* label, int* v, int speed, int v_min, int v_max, const char* fmt = "%d");

    bool Selectable(const char* label, bool selected);

    void NextLine(); // 줄바꿈
}
