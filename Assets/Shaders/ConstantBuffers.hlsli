#ifndef YUNO_CB_INCLUDE
#define YUNO_CB_INCLUDE

// 오브젝트 단위 ------------------------------------------------
cbuffer CBPerObject_Matrix : register(b0)
{
    float4x4 mWorld;
    float4x4 mWVP;
    float4x4 mWInvT;

};

cbuffer CBPerObject_Material : register(b1)
{
    float4 gBaseColor;
    
    float gRoughRatio; 
    float gMetalRatio; // 맵 적용 비율
    
    float gShadowBias;
    
    float gPadding;
}

// 프레임 단위 ------------------------------------------------
cbuffer CBPerFrame : register(b2)
{
    //camera pos ㄱㄱ
    float4x4 mView;
    float4x4 mProj;
    float3 camPos_W;
    
    float pad;
};

// 라이트 (b3) >> Light.hlsli에 있음
// 쉐도우
// 본
// 안개

#endif

