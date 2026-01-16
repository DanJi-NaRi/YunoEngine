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

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    
    float4 pos = mul(float4(i.pos, 1.0f), mWVP);
    float4 pos_W = mul(float4(i.pos, 1.0f), mWorld);

    
    
    
    o.pos = pos;
    o.pos_W = pos_W;
    o.nrm = i.nrm;
    o.uv = i.uv;
    o.T = i.T;
    o.B = i.B;

    
    return o;
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
    float4 nrm_TBN = normalize(float4(ComputeBumpNormal(normalMap, input.T, input.nrm), 0));
    float4 nrm_W = mul(nrm_TBN, mWorld);
    
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
    
    return finalColor;
}
