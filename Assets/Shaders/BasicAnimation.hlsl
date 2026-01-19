#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD0;
    float4 weight : BONEWEIGHT0;
    uint4 index : BONEINDICES0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD0;
};

float4 Skinning(float4 pos, float4 weight, uint4 index);
float3 SkinningNrm(float3 nrm, float4 weight, uint4 index);

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    
    float4 pos = float4(i.pos, 1);
    
    pos = Skinning(pos, i.weight, i.index);
    float3 nrm = SkinningNrm(i.nrm, i.weight, i.index);
    
    nrm = mul(nrm, (float3x3) mWInvT);
    nrm = normalize(nrm);
    
    o.pos = mul(pos, mWVP);
    o.nrm = nrm;
    o.uv = i.uv;
    return o;
}

float4 Skinning(float4 pos, float4 weight, uint4 index)
{
    float4 skinVtx;
    
    float4 v0 = mul(pos, mBoneAnimation[index.x]);
    float4 v1 = mul(pos, mBoneAnimation[index.y]);
    float4 v2 = mul(pos, mBoneAnimation[index.z]);
    float4 v3 = mul(pos, mBoneAnimation[index.w]);
    
    skinVtx = v0 * weight.x
                  + v1 * weight.y
                  + v2 * weight.z
                  + v3 * weight.w;

    return skinVtx;
}

float3 SkinningNrm(float3 nrm, float4 weight, uint4 index)
{
    float3 skinNrm;
    
    float3 n0 = mul(nrm, (float3x3) mBoneAnimation[index.x]);
    float3 n1 = mul(nrm, (float3x3) mBoneAnimation[index.y]);
    float3 n2 = mul(nrm, (float3x3) mBoneAnimation[index.z]);
    float3 n3 = mul(nrm, (float3x3) mBoneAnimation[index.w]);
    
    skinNrm = n0 * weight.x
                    + n1 * weight.y
                    + n2 * weight.z
                    + n3 * weight.w;

    return normalize(skinNrm);
}


float4 PSMain(VSOutput input) : SV_Target
{
    float4 tex = gAlbedo.Sample(SamplerWrap, input.uv);
    float4 col = gBaseColor * tex;

    return col;
}
