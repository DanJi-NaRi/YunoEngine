#pragma once

struct Vec3Desc
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct TransformDesc
{
    Vec3Desc position{ 0, 0, 0 };
    Vec3Desc rotation{ 0, 0, 0 };   // Euler (deg or rad 통일)
    Vec3Desc scale{ 1, 1, 1 };
};

using ObjectID = uint64_t;

struct UnitDesc
{
    ObjectID ID;
    ObjectID parentID = 0;
    std::wstring name;
    std::wstring meshPath;
    
    TransformDesc transform;

    std::wstring unitType;
};

struct DirectionalLightDesc;
struct PointLightDesc;

struct SceneDesc
{
    int version = 1;

    std::wstring sceneName;

    bool isOrtho;

    std::vector<UnitDesc> units;
    std::optional<DirectionalLightDesc> dirLight;
    std::vector<PointLightDesc> pointLights;
};

