#pragma once
#include <cstdint>

using MaterialHandle = uint32_t;
using TextureHandle = uint32_t;

static constexpr MaterialHandle kInvalidMaterialHandle = 0;
static constexpr TextureHandle  kInvalidTextureHandle = 0;

enum class AlphaMode : uint8_t
{
    Opaque = 0,
    Mask,
    Blend,
};


struct PBRMaterialDesc
{
    // Factors
    float baseColorFactor[4] = { 1.f, 1.f, 1.f, 1.f }; // RGBA
    float emissiveFactor[3] = { 0.f, 0.f, 0.f };      // RGB
    float metallicFactor = 1.f;
    float roughnessFactor = 1.f;
    float normalScale = 1.f;
    float occlusionStrength = 1.f;

    // Alpha
    AlphaMode alphaMode = AlphaMode::Opaque;
    float alphaCutoff = 0.5f;   // Mask일 때 사용
    bool doubleSided = false;

    // Textures (핸들 기반)
    TextureHandle baseColorTex = kInvalidTextureHandle;
    TextureHandle normalTex = kInvalidTextureHandle;
    TextureHandle metallicRoughnessTex = kInvalidTextureHandle; // (R=Metallic, G=Roughness) 관례
    TextureHandle occlusionTex = kInvalidTextureHandle;
    TextureHandle emissiveTex = kInvalidTextureHandle;


};
