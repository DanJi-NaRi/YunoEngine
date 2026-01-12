#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct YunoTransform
{
    XMFLOAT3 position{ 0,0,0 };
    XMFLOAT3 rotation{ 0,0,0 };
    XMFLOAT3 scale{ 1,1,1 };

    XMMATRIX ToMatrix() const
    {

        const XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
        const XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
        const XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);
        return S * R * T;
    }
};
