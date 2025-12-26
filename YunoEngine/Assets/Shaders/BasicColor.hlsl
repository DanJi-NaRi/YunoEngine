cbuffer PerFrame : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
    float4x4 mWVP;
};

struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VSOutput VSMain(VSInput v)
{
    VSOutput o;

    float4 pos = float4(v.pos, 1.0f);
    float4 wpos = mul(pos, mWorld);
    float4 vpos = mul(wpos, mView);
    float4 ppos = mul(vpos, mProj);
    
    o.pos = ppos;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET
{

    // 간단한 단색 머티리얼
    return float4(1.0f, 0.35f, 0.2f, 1.0f);
}