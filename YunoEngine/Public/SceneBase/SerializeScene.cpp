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

    inline void from_json(const json& j, Vec3Desc& v)
    {
        // 배열 형태: [x, y, z]
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    inline void to_json(json& j, const Vec4Desc& v)
    {
        j = json::array({ v.x, v.y, v.z, v.w });
    }

    inline void from_json(const json& j, Vec4Desc& v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
        v.w = j.at(3).get<float>();
    }

    inline void to_json(json& j, const TransformDesc& t)
    {
        j = {
            { "position", t.position },
            { "rotation", t.rotation },
            { "scale",    t.scale }
        };
    }

    inline void from_json(const json& j, TransformDesc& t)
    {
        j.at("position").get_to(t.position);
        j.at("rotation").get_to(t.rotation);
        j.at("scale").get_to(t.scale);
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

    inline void from_json(const json& j, UnitDesc& d)
    {
        j.at("ID").get_to(d.ID);
        j.at("parentID").get_to(d.parentID);

        d.name = Utf8ToWString(j.at("name").get<std::string>());
        d.meshPath = Utf8ToWString(j.at("meshPath").get<std::string>());
        d.unitType = Utf8ToWString(j.at("unitType").get<std::string>());

        j.at("transform").get_to(d.transform);
    }

    inline void to_json(json& j, const PointLightDescSave& d)
    {
        j = {
        { "lightpos",  d.lightpos },
        { "lightCol",  d.lightCol },
        { "intensity", d.intensity }
        };
    }

    inline void from_json(const json& j, PointLightDescSave& d)
    {
        j.at("lightpos").get_to(d.lightpos);
        j.at("lightCol").get_to(d.lightCol);
        j.at("intensity").get_to(d.intensity);
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

    std::vector<PointLightDesc> ConvertToLoad(const std::vector<PointLightDescSave>& pds)
    {
        std::vector<PointLightDesc> pd;
        pd.reserve(pds.size());

        for (auto& pls : pds)
        {
            PointLightDesc pl;
            pl.lightCol = ToXM(pls.lightCol);
            pl.lightpos = ToXM(pls.lightpos);
            pl.intensity = pls.intensity;
            pd.push_back(pl);
        }

        return pd;
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

    inline void from_json(const json& j, DirectionalLightDescSave& d)
    {
        j.at("direction").get_to(d.direction);
        j.at("diff").get_to(d.diff);
        j.at("amb").get_to(d.amb);
        j.at("spec").get_to(d.spec);
        j.at("intensity").get_to(d.intensity);
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

    DirectionalLightDesc ConvertToLoad(const DirectionalLightDescSave& dds)
    {
        DirectionalLightDesc dl;

        dl.direction = ToXM(dds.direction);
        dl.diff = ToXM(dds.diff);
        dl.amb = ToXM(dds.amb);
        dl.spec = ToXM(dds.spec);
        dl.intensity = dds.intensity;

        return dl;
    }

    inline void to_json(json& j, const SceneDesc& s)
    {
        auto pls = ConvertToSave(s.pointLights);
        auto dir = ConvertToSave(*s.dirLight);

        std::string name = WStringToUtf8(s.sceneName);

        j = {
            { "version",     s.version },
            { "scenename",     name },
            { "isOrtho",     s.isOrtho },
            { "units",       s.units },
            { "pointLights", pls }
        };

        if (s.dirLight)
            j["dirLight"] = dir;
    }

    inline void from_json(const json& j, SceneDesc& s)
    {
        j.at("version").get_to(s.version);
        s.sceneName = Utf8ToWString(j.at("scenename").get<std::string>());
        j.at("isOrtho").get_to(s.isOrtho);

        j.at("units").get_to(s.units);

        // Directional light는 optional
        if (j.contains("dirLight"))
            s.dirLight = ConvertToLoad(j.at("dirLight").get<DirectionalLightDescSave>());
        else
            s.dirLight.reset();

        if (j.contains("pointLights"))
            s.pointLights = ConvertToLoad(j.at("pointLights").get<std::vector<PointLightDescSave>>());
    }

    void SaveSceneToFile(const SceneDesc& scene, const std::wstring& path)
    {
        json j = scene;

        std::filesystem::path p(path);
        std::filesystem::create_directories(p.parent_path());

        std::ofstream ofs(p);
        if (!ofs.is_open())
        {
            std::wcerr << L"Failed to open file: " << path << std::endl;
            return;
        }

        ofs << j.dump(4);
    }

    bool LoadSceneFromFile(std::unique_ptr<ObjectManager>& om, const std::wstring& path)
    {
        namespace fs = std::filesystem;

        fs::path p(path);

        if (!fs::exists(p))
        {
            std::wcerr << L"Scene file not found: " << path << std::endl;
            return false;
        }

        std::ifstream ifs(p);
        if (!ifs.is_open())
        {
            std::wcerr << L"Failed to open scene file: " << path << std::endl;
            return false;
        }

        json j;
        try
        {
            ifs >> j;
        }
        catch (const std::exception& e)
        {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return false;
        }

        SceneDesc scene;
        try
        {
            scene = j.get<SceneDesc>();
        }
        catch (const std::exception& e)
        {
            std::cerr << "SceneDesc deserialize error: " << e.what() << std::endl;
            return false;
        }

        om->Clear();

        om->SetOrthoFlag(scene.isOrtho);

        for (const UnitDesc& d : scene.units)
        {
            om->CreateObjectFromDesc(d);
        }

        if (scene.dirLight)
            om->CreateDirLightFromDesc(*scene.dirLight);

        for (const auto& pl : scene.pointLights)
            om->CreatePointLightFromDesc(pl);

        return true;
    }
}

