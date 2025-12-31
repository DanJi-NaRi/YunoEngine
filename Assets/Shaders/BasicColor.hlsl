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
    float3 uvColor = float3(input.uv, 0.0f); // (u, v, 0)
    return float4(uvColor, 1.0f) * gBaseColor;
}
