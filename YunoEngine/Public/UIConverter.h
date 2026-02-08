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



inline Float3 LocalToWorldPos(const Float3& localPos, const Float3& parentPos, const Float3& parentScale)
{
    return { localPos.x * parentScale.x + parentPos.x,
             localPos.y * parentScale.y + parentPos.y,
             localPos.z * parentScale.z + parentPos.z };
}

inline Float3 WorldToLocalPos(const Float3& worldPos, const Float3& parentPos, const Float3& parentScale)
{
    return { (worldPos.x - parentPos.x) / parentScale.x,
             (worldPos.y - parentPos.y) / parentScale.y,
             (worldPos.z - parentPos.z) / parentScale.z };
}

inline XMFLOAT3 LocalToWorldPos(const XMFLOAT3& localPos, const XMFLOAT3& parentPos, const XMFLOAT3& parentScale)
{
    return { localPos.x * parentScale.x + parentPos.x,
             localPos.y * parentScale.y + parentPos.y,
             localPos.z * parentScale.z + parentPos.z };
}

inline XMFLOAT3 WorldToLocalPos(const XMFLOAT3& worldPos, const XMFLOAT3& parentPos, const XMFLOAT3& parentScale)
{
    return { (worldPos.x - parentPos.x) / parentScale.x,
             (worldPos.y - parentPos.y) / parentScale.y,
             (worldPos.z - parentPos.z) / parentScale.z };
}

inline XMFLOAT3 TransformPoint(const XMFLOAT3& pos, const XMMATRIX& matrix)
{
    const XMVECTOR p = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
    const XMVECTOR t = XMVector4Transform(p, matrix);
    return { XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) };
}

inline XMFLOAT3 WorldToLocalPos(const XMFLOAT3& worldPos, const XMMATRIX& parentWorld)
{
    const XMMATRIX invParent = XMMatrixInverse(nullptr, parentWorld);
    return TransformPoint(worldPos, invParent);
}
