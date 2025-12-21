#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CBDefault
{
    XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;
};

