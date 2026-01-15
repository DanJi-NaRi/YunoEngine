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


// 이때 넣는 nrm은 local입니다.  
float4 DirLight(float4 nrm)
{
    float4 N = nrm;
    N.w = 0;
    float4 L = dirLight.Dir;
    L.w = 0;

    float4 diff = max(dot(N, L), 0) * dirLight.Diff;
    float4 amb = dirLight.Amb;
    
    return diff + amb;
}



#endif

