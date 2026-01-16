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
    
    
    
       // 조명 계산 <방식2> : 반사율(표면 거칠기 Roughness ) : rough.map 적용     
    //                     (데모용, smooth 가 1 이면, 레거시 specular 계산)
	//아래의 조건 각각 테스트 해 볼 것.★        
	//float4 spec = lt * pow(max(dot(N, H), 0.0f), rough * 100.0f);    //rough 적용 
    //float4 spec = lt * pow(max(dot(N, H), 0.0f), smooth * 100.0f);    //smooth 적용  풀질비교
    //if(smooth.a) spec *= smooth;                                      //smooth 적용(2). 품질비교 요망.
    //return spec;
      
    
    
    // 조명 계산 <방식3> : Cook-Torrance BRDF 모델 적용
    // 1. Frenel 적용
    // 나머지는 기본값 처리.
    // [참고] 프레넬 굴절율 (IOR, List of Relfraction) (https://pixelandpoly.com/ior.html)
    // 공기(Air) : 1.0
    // 금속 (iron) : 2.950
    // 알루미늄(Aluminum) : 1.390
    // 금(Gold) : 0.470
    // 얼름(Ice) : 1.309
    // 플라스틱(Plastic) : 1.460
    // 물(Water) : 1.325
    // 티타늄(Titanium) : 2.160
    // 유리(Glass) : 1.500
    // 크리스탈(Crystal) : 2.000
    // 자동차 페인트 : 1.175
    //
    //아래의 조건 각각 테스트 해 볼 것.★
    //float4 R0 = float4(0.02, 0.02f, 0.02, 0);         //Water
    //float4 R0 = float4(0.08, 0.08f, 0.08, 0);         //Glass
    //float4 R0 = float4(0.05, 0.05f, 0.05, 0);         //Plastic
    //float4 R0 = float4(1.00, 0.71f, 0.29, 0); //Gold -> Fresnel 반사 테스트 추천.★
    float4 R0 = float4(0.95, 0.93f, 0.88, 0); //Silver
    //float4 R0 = float4(0.95, 0.64f, 0.54, 0);         //Copper
    //float4 R0 = float4(0.24, 0.24f, 0.24, 1);         //Iron
    //float4 R0 = float4(1, 0.0f, 0.0, 1);              //Iron (Red) 테스트용
    //float4 R0 = float4(0.0064, 0.0064, 0.0064, 0);     //자동차 페인트..(??)
    
    R0 *= metal * smooth;
    
    
 //굴절율, 텍스처 기반 처리.
    //float4 R0 = base;
    //float4 R0 = spec0;
    
    
    //아래의 조건 각각 테스트 해 볼 것.★
    float4 D = PBR_D(N, H, rough.x);
    //float4 D = 1;
    float4 F = PBR_F(R0, V, H);
    float4 G = PBR_G(V, N, rough.x);
    //float4 G = 1;
    //float n_l = dot(N, L);
    //float n_v = dot(N, V);
    //float n_l = max(dot(N, L), 0);
    //float n_v = max(dot(N, V), 0);    
    float n_l = saturate(max(dot(N, L), 0.001));
    float n_v = saturate(max(dot(N, V), 0.001));
    
    
    //아래의 조건 각각 테스트 해 볼 것.★
    //float4 specBRDF = (D * F * G)/(2 * n_l * n_v);            //2배 배율 (더 넓은 확산)
    float4 specBRDF = (D * F * G) / (4 * n_l * n_v); //Cook-Torrance 표준.
    //float4 specBRDF = (D * F * G)/(8 * PI * n_l * n_v);       //2π 감쇄 배울 추가.
    //float4 specBRDF = (D * F * G)/(16 * PI * n_l * n_v);      //4π 감쇄 배울 추가. (더 선명한 반사)
    //float4 specBRDF = G_CookTorrance(V.xyz, N.xyz, L.xyz, rough.x);
        
    
    //아래의 조건 각각 테스트 해 볼 것.★
    //float4 spec = F;
    //float4 spec = F * base;
    //float4 spec = specBRDF;
    //float4 spec = saturate(specBRDF);
    //float4 spec = g_Lit.Specular * specBRDF;
    //float4 spec = lt * specBRDF;
    //float4 spec = lt * saturate(specBRDF);          //기본멥+조명+Specular 혼합 ★
    float4 spec = lt * saturate(specBRDF) * smooth;
    //float4 spec = lt * smooth + saturate(specBRDF) * smooth;
    
    // 반사율(표면 거칠기 Roughness ) : rough.map 적용     
    //float4 spec = lt * pow(max(dot(N, H), 0.0f), rough * 100.0f);       //rough 적용 
    //return specBRDF;
    return spec;
    
}

#endif

