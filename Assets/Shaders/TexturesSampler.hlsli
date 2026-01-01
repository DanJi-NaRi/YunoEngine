#ifndef YUNO_TS_INCLUDE
#define YUNO_TS_INCLUDE

// Texture slots
Texture2D gAlbedo : register(t0);
Texture2D gNormal : register(t1);
Texture2D gORM    : register(t2);

// Sampler slots
SamplerState SamplerCustom      : register(s0);
SamplerState SamplerWrap        : register(s1);
SamplerState SamplerClamp       : register(s2);
SamplerState SamplerBorder0000  : register(s3);

#endif

