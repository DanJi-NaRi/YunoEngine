#pragma once
#include "YunoTransform.h"

struct Vec2Desc
{
    float x = 0.f;
    float y = 0.f;
};

struct Vec3Desc
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct Vec4Desc
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;
};

struct TransformDesc
{
    Vec3Desc position{ 0, 0, 0 };
    Vec3Desc rotation{ 0, 0, 0 };   // Euler (deg or rad 통일)
    Vec3Desc scale{ 1, 1, 1 };
};

inline Vec2Desc FromFloat(const Float2& xm)
{
    return Vec2Desc{ xm.x, xm.y };
}

inline Float2 ToFloat(const Vec2Desc& vd)
{
    return Float2(vd.x, vd.y);
}

inline Float3 ToFloat(const Vec3Desc& vd)
{
    return Float3(vd.x, vd.y, vd.z);
}

inline Vec3Desc FromFloat(const Float3& xm)
{
    return Vec3Desc{ xm.x, xm.y, xm.z };
}

inline Float4 ToFloat(const Vec4Desc& vd)
{
    return Float4(vd.x, vd.y, vd.z, vd.w);
}

inline Vec4Desc FromFloat(const Float4& xm)
{
    return Vec4Desc{ xm.x, xm.y, xm.z, xm.w };
}


inline XMFLOAT2 ToXM(const Vec2Desc& vd)
{
    return XMFLOAT2(vd.x, vd.y);
}

inline Vec2Desc FromXM(const XMFLOAT2& xm)
{
    return Vec2Desc{ xm.x, xm.y };
}

inline XMFLOAT3 ToXM(const Vec3Desc& vd)
{
    return XMFLOAT3(vd.x, vd.y, vd.z);
}

inline Vec3Desc FromXM(const XMFLOAT3& xm)
{
    return Vec3Desc{ xm.x, xm.y, xm.z };
}


inline XMFLOAT4 ToXM(const Vec4Desc& vd)
{
    return XMFLOAT4(vd.x, vd.y, vd.z, vd.w);
}

inline Vec4Desc FromXM(const XMFLOAT4& xm)
{
    return Vec4Desc{ xm.x, xm.y, xm.z, xm.w };
}

using ObjectID = uint64_t;

struct MeshDesc
{
    UINT meshNum;

    Vec4Desc emissiveCol;
    float emissive;
};

struct UnitDesc
{
    ObjectID ID;
    ObjectID parentID = 0;
    std::wstring name;
    std::wstring meshPath = L"None";
    
    TransformDesc transform;
    std::vector<MeshDesc> MatDesc;

    std::wstring unitType;
};

struct WidgetDesc
{
    ObjectID ID;
    ObjectID parentID = 0;
    std::wstring name;

    Vec2Desc size{ 0, 0 };
    TransformDesc transform;
};

struct DirectionalLightDesc
{
    XMFLOAT4 direction = { 0.58f, -0.58f, 0.58f, 0.0f };

    XMFLOAT4 diff = { 1.0f, 1.0f, 1.0f, 1.0f };

    XMFLOAT4 amb = { 0.2f, 0.2f, 0.2f, 1.0f };

    XMFLOAT4 spec = { 1.0f, 1.0f, 1.0f, 1.0f };

    float intensity = 3.0f;
};

struct DirectionalLightDescSave
{
    Vec4Desc direction = { 0.58f, -0.58f, 0.58f, 0.0f };

    Vec4Desc diff = { 1.0f, 1.0f, 1.0f, 1.0f };

    Vec4Desc amb = { 0.2f, 0.2f, 0.2f, 1.0f };

    Vec4Desc spec = { 1.0f, 1.0f, 1.0f, 1.0f };

    float intensity = 30.0f;
};

struct PointLightDesc
{
    ObjectID id;

    XMFLOAT3 lightpos = { 0, 0, 0 };

    XMFLOAT4 lightCol = { 1.0f, 1.0f, 1.0f, 1.0f };

    float intensity = 30.0f;
};

struct PointLightDescSave
{
    ObjectID id;

    Vec3Desc lightpos = { 0, 0, 0 };

    Vec4Desc lightCol = { 1.0f, 1.0f, 1.0f, 1.0f };

    float intensity = 3.0f;
};

struct PostProcessDesc
{
    uint32_t ppFlag;
    float threshold = 1.01f;
    float bloomIntensity = 0.1f;
    float exposure = 1.3f;
};

struct SceneDesc
{
    int version = 1;

    std::wstring sceneName;

    bool isOrtho;

    PostProcessDesc postprocess;
    std::vector<UnitDesc> units;
    std::optional<DirectionalLightDesc> dirLight;
    std::vector<PointLightDesc> pointLights;
    std::vector<WidgetDesc> widgets;
};

class ObjectManager;
class UIManager;

void SaveSceneToFile(const SceneDesc& scene, const std::wstring& path);

bool LoadSceneFromFile(const std::wstring& path, SceneDesc& out);

