#ifndef YUNO_CB_INCLUDE
#define YUNO_CB_INCLUDE

cbuffer CBPerFrame : register(b0)
{
    float4x4 mView;
    float4x4 mProj;
};

cbuffer CBPerObject : register(b1)
{
    float4x4 mWorld;
    float4x4 mWVP;
    float4x4 mWInvT;
    
    float4 gBaseColor;
    float gRoughRatio;
    float gMetalRatio;
    float gShadowBias;
    //float gPadding[2];
};

#endif

