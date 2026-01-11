#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"


struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 originPos : TEXCOORD0;
};

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos = mul(float4(i.pos, 1.0f), mWVP);
    o.originPos = float4(i.pos, 1.0f);
    return o;
}

float4 PSMain(VSOutput input) : SV_Target
{
    // z축 파란색 출력
    if (input.originPos.z == 0.0f)
        return float4(0, 0, 1, 1);
    
    // x축 빨간색 출력
    if (input.originPos.x == 0.0f)
        return float4(1, 0, 0, 1);

    return float4(0.6, 0.6, 0.6, 1);
}