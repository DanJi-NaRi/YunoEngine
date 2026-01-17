#ifndef YUNO_LIGHT_INCLUDE
#define YUNO_LIGHT_INCLUDE

struct DIR_LIGHT
{
    float4 Dir;         // 빛의 방향.
    float4 Diff;        // 빛의 색
    float4 Amb;         // 주변광
    float4 Spec;
    float intensity;
    float padding[3];
};

cbuffer CBLight : register(b3)
{
    DIR_LIGHT dirLight;
};

#define PI 3.141592f

// 노말 구하는 함수
float3 ComputeBumpNormal(float4 texNorm, float4 T, float3 N)
{
    float3 B;
    float hand = T.w;
    T.xyz = normalize(T.xyz - dot(T.xyz, N) * N);
    B = cross(N, T.xyz) /** hand*/;


    float3 n = texNorm.xyz * 2.0f - 1.0f;
    n.y = -n.y;

    float3x3 mTBN = float3x3(T.xyz, B, N);
    n = normalize(mul(n, mTBN));

    return n;
}


float4 PBR_F(float4 R0, float4 E, float4 H)
{
    float4 F = 0;
    
    float cos = saturate(dot(E, H));
    
    F = R0 + (1 - R0) * pow((1 - cos), 5);
    
    return F;
}

float PBR_D(float4 N, float4 H, float rough)
{
    float a = rough * 128;
    return (a + 2) / (2 * PI) * pow(dot(N, H), a);
}



float PBR_G(float4 V, float4 N, float a)
{
    float k = a * 0.5f;
    float NdotV = dot(N, V);
    return NdotV / (NdotV * (1 - k) + k);
}

// 모든 라이트 연산은 월드에서 계산
// 일반 라이트 연산
float4 DirLight(float4 nrm_W)
{
    float4 N = nrm_W;
    N.w = 0;
    float4 L = normalize(dirLight.Dir);
    L.w = 0;

    float4 diff = max(dot(N, L), 0) * dirLight.Diff;
    float4 amb = dirLight.Amb;
    
    return diff + amb;
}

float4 DirLight(float3 nrm_W)
{
    float4 N = float4(nrm_W, 0);
    float4 L = normalize(dirLight.Dir);
    L.w = 0;

    float4 diff = max(dot(N, L), 0) * dirLight.Diff;
    float4 amb = dirLight.Amb;
    
    return diff + amb;
}


float4 SpecLight(float4 nrm_W, float3 campos_W, float4 pos_W)
{
    float4 spec = 0;
    
    float4 N = normalize(nrm_W);
    
    float4 L = normalize(dirLight.Dir);
    
    float4 worldDir = normalize(float4(campos_W - pos_W.xyz, 0));
    float4 H = normalize(L + worldDir);
    
    float specFactor = pow(max(dot(N, H), 0), 16.0f);
    
    spec = specFactor * dirLight.Diff * dirLight.intensity;
    
    return spec;
}

// PBR 라이트 연산

float4 Diffuse_BRDF(float4 nrm_W, float4 base, float4 metal, float4 ao)
{
    float4 N = nrm_W;
    N.w = 0;
    N = normalize(N);
    float4 L = normalize(dirLight.Dir);
    
    float4 diff0 = base - base * metal;
    
    float4 albedo = diff0 /** ao*/;
    
    float4 lt = dirLight.Diff * dirLight.intensity;
    
    float4 diff = max(dot(N, L), 0) / PI * lt * albedo;

    float4 amb = dirLight.Amb * dirLight.intensity * albedo;

	return saturate(diff + amb);
}

float4 Specular_BRDF(float4 pos_W, float4 nrm_W, float4 base, float4 metal, float4 rough)
{
    float4 P = pos_W;
    P.w = 0;
    float4 N = nrm_W;
    N.w = 0;
    float4 L = normalize(dirLight.Dir);
    N = normalize(N);

    float4 V = normalize(-P);

    float4 H = normalize(L + V);
    
    float4 spec0 = lerp(0.08 * dirLight.Spec, base, metal);
    
    float4 lt = spec0 * dirLight.intensity * dirLight.Diff;
    
    float4 smooth = saturate(1 - rough);
    
    float4 R0 = float4(1.00, 0.71f, 0.29, 0);

    
    R0 *= metal * smooth;
    
    
    float4 D = PBR_D(N, H, rough.x);
    float4 F = PBR_F(R0, V, H);
    float4 G = PBR_G(V, N, rough.x);
    
    float n_l = saturate(max(dot(N, L), 0.001));
    float n_v = saturate(max(dot(N, V), 0.001));
    
    
    float4 specBRDF = (D * F * G) / (4 * n_l * n_v); 

        
    
    float4 spec = lt * saturate(specBRDF) * smooth;

    

    return spec;
    
}

#endif

