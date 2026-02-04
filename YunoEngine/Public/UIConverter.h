#pragma once
#include "YunoTransform.h"

struct Float2;
struct Float3;
struct Float4;

class UIManager;

struct UICanvasMapping
{
    Float2 origin;            // 디자인 해상도 (예: 1920x1080)
    Float2 canvas;            // 현재 클라이언트/캔버스 크기
    Float2 canvasScale;       // (s,s)
    Float2 letterboxOffset;   // 레터박스 중앙정렬 오프셋 (적용 여부는 호출 측에서 선택 가능)
    bool   valid;             // origin/canvas 유효 여부
};




inline Float2 LocalToWorldPos(const Float2& localPos, const Float2& parentPos, const Float2& parentScale)
{
    return { localPos.x * parentScale.x + parentPos.x,
             localPos.y * parentScale.y + parentPos.y };
}

inline Float2 WorldToLocalPos(const Float2& worldPos, const Float2& parentPos, const Float2& parentScale)
{
    return { (worldPos.x - parentPos.x) / parentScale.x,
             (worldPos.y - parentPos.y) / parentScale.y };
}
