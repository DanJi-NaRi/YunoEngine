#pragma once
#include <DirectXMath.h>
#include "IWindow.h"
using namespace DirectX;

struct YunoCamera
{
    XMFLOAT3 position{ 0, 10.0f, -20.0f };
    XMFLOAT3 target{ 0, 0,  0.0f };
    XMFLOAT3 up{ 0, 1,  0.0f };

    float fovY = XM_PIDIV4; 
    float aspect = 1.0f;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    bool useOrtho = false;

    XMMATRIX View() const
    {
        using namespace DirectX;
        return (useOrtho) ? XMMatrixIdentity() : XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), XMLoadFloat3(&up));
    }

    XMMATRIX Proj(float width, float height) const
    {
        using namespace DirectX;
        return (useOrtho) ? XMMatrixOrthographicOffCenterLH(0.0f, (float)width, (float)height, 0.0f, nearZ, farZ) : XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }

    XMMATRIX ProjPerspective() const
    {
        using namespace DirectX;
        return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }

    XMMATRIX ProjOrtho(float width, float height) const
    {
        using namespace DirectX;
        return XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
        //return XMMatrixOrthographicOffCenterLH(0, width, height, 0.0f, 1.0f, 100.0f);
    }

    void SetOrthoFlag(bool useOrtho) { this->useOrtho = useOrtho; } // 투영 활용 플래그 Set 함수.

    XMFLOAT3 Position() const { return position; }
};
