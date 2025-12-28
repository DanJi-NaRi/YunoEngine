cbuffer CBDefault : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
    float4x4 mWVP;
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

float4 PSMain(VSOutput i) : SV_Target
{
    // 임시: UV 기반 컬러
    return float4(i.uv, 0.0f, 1.0f);
}
