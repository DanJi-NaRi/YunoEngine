#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"
#include "Light.hlsli"

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
    
    float4 pos = mul(float4(i.pos, 1.0f), mWVP);
    
    
    
    
    
    o.pos = pos;
    o.nrm = i.nrm;
    o.uv = i.uv;
    return o;
}


float4 PSMain(VSOutput input) : SV_Target
{
    float4 albedoMap = gAlbedo.Sample(SamplerClamp, input.uv);
    float4 normalMap = gNormal.Sample(SamplerClamp, input.uv);
    float4 orm = gORM.Sample(SamplerClamp, input.uv);
    float4 metalMap = gMetallic.Sample(SamplerClamp, input.uv);
    float4 roughMap = gRoughness.Sample(SamplerClamp, input.uv);
    float4 aoMap = gAO.Sample(SamplerClamp, input.uv);
    
    
    float4 col = gBaseColor * albedoMap;
    return roughMap;
}
