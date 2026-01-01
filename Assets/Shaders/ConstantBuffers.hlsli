#ifndef YUNO_CB_INCLUDE
#define YUNO_CB_INCLUDE

cbuffer CBDefault : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
    float4x4 mWVP;
};

cbuffer CBMaterial : register(b1)
{
    float4 gBaseColor;
    float gRoughness;
    float gMetallic;
    uint gFlags;
    float gPadding;
};

#endif

