#include "pch.h"

#include "SerializeScene.h"
#include "json.hpp"
#include "YunoLight.h"
#include "ObjectManager.h"

inline std::string WStringToUtf8(const std::wstring& w)
{
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), size, nullptr, nullptr);
    return s;
}

inline std::wstring Utf8ToWString(const std::string& s)
{
    if (s.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), size);
    return w;
}

namespace nlohmann
{
    inline void to_json(json& j, const Vec3Desc& v)
    {
        j = json::array({ v.x, v.y, v.z });
    }

    inline void to_json(json& j, const TransformDesc& t)
    {
        j = {
            { "position", t.position },
            { "rotation", t.rotation },
            { "scale",    t.scale }
        };
    }

    inline void to_json(json& j, const UnitDesc& d)
    {
        j = {
            { "ID",        d.ID },
            { "parentID",  d.parentID },
            { "name",      WStringToUtf8(d.name) },
            { "meshPath",  WStringToUtf8(d.meshPath) },
            { "unitType",  WStringToUtf8(d.unitType) },
            { "transform", d.transform }
        };
    }

    std::vector<PointLightDescSave> ConvertToSave(const std::vector<PointLightDesc>& pd)
    {
        std::vector<PointLightDescSave> pds;
        pds.reserve(pd.size());

        for (auto& pl : pd)
        {
            PointLightDescSave pls;
            pls.lightCol = FromXM(pl.lightCol);
            pls.lightpos = FromXM(pl.lightpos);
            pls.intensity = pl.intensity;
            pds.push_back(pls);
        }

        return pds;
    }

    inline void to_json(json& j, const Vec4Desc& v)
    {
        j = json::array({ v.x, v.y, v.z, v.w });
    }

    inline void to_json(json& j, const PointLightDescSave& d)
    {
        j = {
        { "lightpos",  d.lightpos },
        { "lightCol",  d.lightCol },
        { "intensity", d.intensity }
        };
    }

    DirectionalLightDescSave ConvertToSave(const DirectionalLightDesc& pd)
    {
        DirectionalLightDescSave dls;

        dls.direction = FromXM(pd.direction);
        dls.diff = FromXM(pd.diff);
        dls.amb = FromXM(pd.amb);
        dls.spec = FromXM(pd.spec);
        dls.intensity = pd.intensity;

        return dls;
    }

    inline void to_json(json& j, const DirectionalLightDescSave& d)
    {
        j = {
        { "direction",  d.direction },
        { "diff",  d.diff },
        { "amb",  d.amb },
        { "spec", d.spec },
        { "intensity", d.intensity }
        };
    }

    inline void to_json(json& j, const SceneDesc& s)
    {
        auto pls = ConvertToSave(s.pointLights);
        auto dir = ConvertToSave(*s.dirLight);

        j = {
            { "version",     s.version },
            { "isOrtho",     s.isOrtho },
            { "units",       s.units },
            { "pointLights", pls }
        };

        if (s.dirLight)
            j["dirLight"] = dir;
    }

    void SaveSceneToFile(const SceneDesc& scene, const std::wstring& path)
    {
        json j = scene;

        std::ofstream ofs(path);
        ofs << j.dump(4); // 보기 좋게
    }

    void SceneLoad(const ObjectManager& om)
    {

    }
}

