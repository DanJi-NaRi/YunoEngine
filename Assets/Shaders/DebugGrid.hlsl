#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"


struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};
VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos = mul(float4(i.pos, 1.0f), mWVP);
    return o;
}

float4 PSMain(VSOutput input) : SV_Target
{
    // 엔진 머티리얼에서 gBaseColor 쓰고 싶으면 아래로 교체 가능
    // return gBaseColor;
    return float4(1, 1, 1, 1); // 일단 노랑 고정
}