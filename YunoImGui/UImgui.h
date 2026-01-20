#pragma once

namespace UI
{
    void SetNextUIPos(UINT x, UINT y);
    void SetNextUISize(UINT width, UINT height);

    void BeginPanel(const char* name);
    void EndPanel();

    bool Button(const char* label);
    void Text(const char* fmt, ...);

    bool Selectable(const char* label, bool selected);
}
