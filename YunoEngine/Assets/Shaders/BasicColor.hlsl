cbuffer PerFrame : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
    float4x4 mWVP;
};

struct VSInput
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VSOutput VSMain(VSInput v)
{
    VSOutput o;

    float4 pos = v.pos;
    float4 wpos = mul(pos, mWorld);
    float4 vpos = mul(wpos, mView);
    float4 ppos = mul(vpos, mProj);
    
    o.pos = ppos;
    o.col = v.col;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET
{
    return i.col;
}
