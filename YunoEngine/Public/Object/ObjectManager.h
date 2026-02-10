#pragma once

#include "RenderTypes.h"
#include "Unit.h"
#include "AnimationUnit.h"
#include "SerializeScene.h"
#include "Effect.h"

class YunoDirectionalLight;
class YunoPointLight;
class IEffectManager;

class ObjectManager
{
private:
    using CreateFn = std::function<std::unique_ptr<Unit>(ObjectManager&, const UnitDesc&)>;
    struct MeshCacheKey
    {
        std::wstring filepath;
        PassOption opt;

        bool operator==(const MeshCacheKey& rhs) const
        {
            return filepath == rhs.filepath
                && opt.shader == rhs.opt.shader
                && opt.blend == rhs.opt.blend
                && opt.raster == rhs.opt.raster
                && opt.depth == rhs.opt.depth;
        }
    };

    struct MeshCacheKeyHash
    {
        size_t operator()(const MeshCacheKey& key) const
        {
            size_t hash = std::hash<std::wstring>{}(key.filepath);
            auto hashCombine = [&hash](size_t value)
                {
                    hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                };

            hashCombine(static_cast<size_t>(key.opt.shader));
            hashCombine(static_cast<size_t>(key.opt.blend));
            hashCombine(static_cast<size_t>(key.opt.raster));
            hashCombine(static_cast<size_t>(key.opt.depth));
            return hash;
        }
    };

    size_t m_objectCount;
    UINT m_objectIDs;
    UINT m_pointLightIDs;

    std::deque<std::unique_ptr<Unit>> m_objs;

    std::unordered_map<UINT, Unit*> m_objMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;
    std::unordered_map<MeshCacheKey, std::unique_ptr<MeshNode>, MeshCacheKeyHash> m_meshCache;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;
   
    //Camera 투영
    bool m_isOrtho = false;

    std::unique_ptr<MeshNode> CreateMeshNode(const std::wstring& filepath, PassOption opt);

    std::unique_ptr<YunoDirectionalLight> m_directionLight;
    std::vector<std::unique_ptr<YunoPointLight>> m_pointLights;
    IEffectManager* m_effectManager = nullptr;

public:
    void CreateDirLight();
    void CreatePointLight(const XMFLOAT3& pos, const XMFLOAT4& col, float intensity);
    void CreateDirLightFromDesc(const DirectionalLightDesc& dd);
    void CreatePointLightFromDesc(const PointLightDesc& pd);
    void SetOrthoFlag(bool flag) { m_isOrtho = flag; };
    void SetEffectManager(IEffectManager* manager) { m_effectManager = manager; }

    YunoDirectionalLight* GetDirLight() { return m_directionLight.get(); }
    std::vector<std::unique_ptr<YunoPointLight>>& GetPointLights() { return m_pointLights; }

public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    bool Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출

    void Update(float dTime);
    void Submit(float dTime);

    void CreateObjectFromDesc(const UnitDesc& desc);
    template<typename T>
    void CreateObjectInternal(const UnitDesc& desc);
    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos, UINT id = 0);
    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos, PassOption opt, UINT id = 0);
    template<typename T>
    T* CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath, PassOption opt = {}, UINT id = 0);

    //씬 매니저에 있어도 될것같은 놈들
    Unit* FindObject(UINT id); //id로 검색
    Unit* FindObject(const std::wstring& name); //이름으로 검색


    void DestroyObject(UINT id);
    void DestroyObject(const std::wstring& name);

    const size_t GetObjectCount() { return m_objectCount; }
    const std::unordered_map<UINT, Unit*>& GetObjectlist() { return m_objMap; }

    SceneDesc BuildSceneDesc();
    void ApplyUnitFromDesc(const std::vector<UnitDesc>& uds);
    void ApplyDirLightFromDesc(const DirectionalLightDesc& dd);
    void ApplyPointLightsFromDesc(const std::vector<PointLightDesc>& pds);
private:
    void CheckDedicateObjectName(std::wstring& name);
    bool IsObjectIDTaken(UINT id) const;
    UINT AllocateObjectID(UINT preferredID = 0);


    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    std::vector<Frame_Data_Point> plData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};

template<typename T>
void ObjectManager::CreateObjectInternal(const UnitDesc& desc)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");
    Unit* obj;
    if (desc.meshPath.empty()) //메쉬파일 없는 오브젝트
    {
        obj = CreateObject<T>(desc.name, ToXM(desc.transform.position), desc.ID);
    }
    else
    {
        obj = CreateObjectFromFile<T>(desc.name, ToXM(desc.transform.position), desc.meshPath, {}, desc.ID);
    }

    auto& degRot = desc.transform.rotation;
    XMFLOAT3 radRot = XMFLOAT3(XMConvertToRadians(degRot.x), 
                                                             XMConvertToRadians(degRot.y), 
                                                             XMConvertToRadians(degRot.z));
    obj->SetRot(radRot);
    obj->SetScale(ToXM(desc.transform.scale));
    obj->SetEffectManager(m_effectManager);

    if (desc.hasEffectEmissive)
    {
        if (auto* effect = dynamic_cast<Effect*>(obj))
        {
            effect->m_emissiveCol = ToXM(desc.effectEmissiveColor);
            effect->m_emissive = desc.effectEmissive;
        }
    }
}

template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos, UINT id)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    UINT newID = AllocateObjectID(id);

    std::wstring newname = name;

    auto obj = std::make_unique<T>();
    CheckDedicateObjectName(newname);

    obj->Create(newname, newID, pos);
    obj->SetEffectManager(m_effectManager);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}

template<typename T>//파싱없이 메쉬 생성해서 렌더할 오브젝트, 렌더패스 옵션 설정가능
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos, PassOption opt, UINT id) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    UINT newID = AllocateObjectID(id);

    std::wstring newname = name;
    
    auto obj = std::make_unique<T>();
    CheckDedicateObjectName(newname);

    obj->Create(newname, newID, pos, opt);
    obj->SetEffectManager(m_effectManager);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}

template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath, PassOption opt, UINT id)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    UINT newID = AllocateObjectID(id);

    auto mesh = CreateMeshNode(filepath, opt);

    auto meshnode = std::move(mesh);

    std::wstring newname = name;

    CheckDedicateObjectName(newname);

    auto obj = std::make_unique<T>();
    obj->Create(newname, newID, pos);
    obj->SetMeshPath(filepath);

    obj->SetMesh(std::move(meshnode));
    obj->SetEffectManager(m_effectManager);

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}

