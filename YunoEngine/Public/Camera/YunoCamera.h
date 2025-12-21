#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct YunoCamera
{
    XMFLOAT3 position{ 0, 1.5f, -3.0f };
    XMFLOAT3 target{ 0, 0,  0.0f };
    XMFLOAT3 up{ 0, 1,  0.0f };

    float fovY = XM_PIDIV4; 
    float aspect = 1.0f;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    XMMATRIX View() const
    {
        using namespace DirectX;
        return XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), XMLoadFloat3(&up));
    }

    XMMATRIX Proj() const
    {
        using namespace DirectX;
        return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }
};
