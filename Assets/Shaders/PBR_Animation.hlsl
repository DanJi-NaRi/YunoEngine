#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"
#include "Light.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD0;
    float4 T : TANGENT0;
    float3 B : BINORMAL0;
    float4 weight : BONEWEIGHT0;
    uint4 index : BONEINDICES0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 pos_W : TEXCOORD0;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD1;
    float4 T : TANGENT;
    float3 B : BINORMAL;
};

float4 Skinning(float4 pos, float4 weight, uint4 index);
float3 SkinningNrm(float3 nrm, float4 weight, uint4 index);

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    float4 pos = float4(i.pos, 1);
    float3 nrm = i.nrm;
    
    pos = Skinning(pos, i.weight, i.index);
    nrm = SkinningNrm(nrm, i.weight, i.index);
    
    pos = mul(pos, mWVP);
    float4 pos_W = mul(pos, mWorld);

    nrm = mul(nrm, (float3x3) mWInvT);
    nrm = normalize(nrm);
    
    o.pos = pos;
    o.pos_W = pos_W;
    o.nrm = nrm;
    o.uv = i.uv;
    o.T = i.T;
    o.B = i.B;

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
    
    float3 n0 = mul(nrm, (float3x3)mBoneAnimation[index.x]);
    float3 n1 = mul(nrm, (float3x3)mBoneAnimation[index.y]);
    float3 n2 = mul(nrm, (float3x3)mBoneAnimation[index.z]);
    float3 n3 = mul(nrm, (float3x3)mBoneAnimation[index.w]);
    
    skinNrm = n0 * weight.x
                    + n1 * weight.y
                    + n2 * weight.z
                    + n3 * weight.w;

    return normalize(skinNrm);
}

float4 PSMain(VSOutput input) : SV_Target
{
    // Sample Textures
    float4 albedoMap = gAlbedo.Sample(SamplerClamp, input.uv);
    float4 normalMap = gNormal.Sample(SamplerClamp, input.uv);
    float4 ormMap = gORM.Sample(SamplerClamp, input.uv);
    float4 metalMap = gMetallic.Sample(SamplerClamp, input.uv);
    float4 roughMap = gRoughness.Sample(SamplerClamp, input.uv);
    float4 aoMap = gAO.Sample(SamplerClamp, input.uv);
    
    // 변수 초기화
    float4 nrm_W = normalize(float4(ComputeBumpNormal(normalMap, input.T, input.nrm), 0));
    //float4 nrm_W = mul(nrm_TBN, mWorld);
    
    // 디렉션 라이트
    float4 diff = DirLight(nrm_W);
    //float4 diff = Diffuse_BRDF(nrm_W, albedoMap, metalMap, aoMap);
    
    // 스페큘러 라이트
    //float4 spec = SpecLight(nrm_W, camPos_W, input.pos_W);
    float4 spec = Specular_BRDF(input.pos_W, nrm_W, albedoMap, metalMap, roughMap);
    
    
    // 베이스 컬러
    float4 BaseColor = gBaseColor * albedoMap;
    
    // 최종 컬러
    float4 finalColor = BaseColor * diff + spec;
    
    return float4(1, 1, 1, 1);
}
