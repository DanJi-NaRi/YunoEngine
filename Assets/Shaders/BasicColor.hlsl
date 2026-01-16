#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos = mul(float4(i.pos, 1.0f), mWVP);
    o.nrm = i.nrm;
    o.uv = i.uv;
    return o;
}


float4 PSMain(VSOutput input) : SV_Target
{
    float4 tex = gAlbedo.Sample(SamplerWrap, input.uv);
    float4 col = gBaseColor * tex;

    return col;
}
