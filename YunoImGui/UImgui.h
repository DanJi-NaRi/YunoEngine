#pragma once

namespace UI
{
    void SetNextUIPos(UINT x, UINT y);
    void SetNextUISize(UINT width, UINT height);

    void BeginPanel(const char* name);
    void EndPanel();

    void Separator();

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

    bool DragFloat3Editable(const char* label, float* v, float speed);

    bool Selectable(const char* label, bool selected);
}
