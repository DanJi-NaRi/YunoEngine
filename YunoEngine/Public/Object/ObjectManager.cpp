#include "pch.h"

#include "ObjectManager.h"
#include "Parser.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "ObjectTypeRegistry.h"


void ObjectManager::CreateObjectFromDesc(const UnitDesc& desc)
{
    auto* fn = ObjectTypeRegistry::Instance().Find(desc.unitType);

    if (!fn)
    {
        std::cerr << "Unknown type(ObjectManager.cpp, 16), Please Register Type" << std::endl;
        return;
    }

    (*fn)(*this, desc);
}

void ObjectManager::CreateDirLight()
{
    if(!m_directionLight)
        m_directionLight = std::make_unique<YunoDirectionalLight>();
}

void ObjectManager::CreatePointLight(const XMFLOAT3& pos, const XMFLOAT4& col, float intensity)
{
    if (m_pointLights.size() >= 10) return;

    PointLightDesc pd{};
    pd.id = m_pointLightIDs++;
    pd.lightpos = pos;
    pd.lightCol = col;
    pd.intensity = intensity;
    
    auto pl = std::make_unique<YunoPointLight>(pd);
    m_pointLights.push_back(std::move(pl));
}

void ObjectManager::CreateDirLightFromDesc(const DirectionalLightDesc& dd)
{
    if (!m_directionLight)
        m_directionLight = std::make_unique<YunoDirectionalLight>(dd);
}

void ObjectManager::CreatePointLightFromDesc(const PointLightDesc& pd)
{
    if (m_pointLights.size() >= 10) return;

    auto pl = std::make_unique<YunoPointLight>(pd);
    m_pointLights.push_back(std::move(pl));
}

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

bool ObjectManager::Init()
{
    m_objectCount = 0;
    m_objectIDs = 1;
    m_pointLightIDs = 1;
    m_objMap.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);

    m_pointLights.reserve(10);
    plData.resize(10);

    return true;
}

void ObjectManager::Clear()
{

    m_objectCount = 0;
    m_objectIDs = 1;
    m_pointLightIDs = 1;
    m_objs.clear(); //오브젝트 객체 완전 삭제
    m_pendingCreateQ.clear();
    m_pendingDestoryQ.clear();
    m_nameToID.clear();
}


//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::Update(float dTime)
{
    FrameDataUpdate();
    for (auto& obj : m_objs)
    {
        obj->Update(dTime);
    }
}
//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::Submit(float dTime)
{
    FrameDataSubmit();

    for (auto& obj : m_objs)
    {
        obj->Submit(dTime);
    }
}

void ObjectManager::ProcessPending()
{
    if (!m_pendingCreateQ.empty())
    {
        for (auto& obj : m_pendingCreateQ)
        {
            UINT id = obj->GetID();
            auto name = obj->GetName();
            auto* pObj = obj.get();
            m_objs.push_back(std::move(obj));
            m_objMap.emplace(id, pObj);
            m_nameToID.emplace(name, id);
            m_objectCount++;
        }

        m_pendingCreateQ.clear();
    }

    if (!m_pendingDestoryQ.empty())
    {
        for (auto& id : m_pendingDestoryQ)
        {
            auto name = m_objMap[id]->GetName();

            auto it = std::find_if(m_objs.begin(), m_objs.end(), [id](const std::unique_ptr<Unit>& obj) { return obj->GetID() == id; });

            if (it == m_objs.end())
                continue;

            it->get()->DettachParent();
            it->get()->ClearChild();

            m_nameToID.erase(name);
            m_objMap.erase(id);

            m_objs.erase(it);
            m_objectCount--;
        }

        m_pendingDestoryQ.clear();
    }
}

//유니크 포인터의 생포인터 반환 외부에서 삭제 절대 금지
Unit* ObjectManager::FindObject(UINT id)//GetID랑 연동해서쓰기
{
    if(m_objMap.find(id) == m_objMap.end())
        return nullptr;

    return m_objMap[id];
}

Unit* ObjectManager::FindObject(const std::wstring& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return nullptr;

    UINT id = m_nameToID[name];

    if (m_objMap.find(id) == m_objMap.end())
        return nullptr;

    return m_objMap[id];
}

void ObjectManager::DestroyObject(UINT id)
{
    if (m_objMap.find(id) == m_objMap.end())
        return;

    for (auto& checkID : m_pendingDestoryQ)//큐에 중복삭제 삽입 방지
    {
        if (id == checkID)
            return;
    }

    m_pendingDestoryQ.push_back(id);
}

void ObjectManager::DestroyObject(const std::wstring& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return;

    UINT id = m_nameToID[name];

    for (auto& checkID : m_pendingDestoryQ)//큐에 중복삭제 삽입 방지
    {
        if (id == checkID)
            return;
    }

    m_pendingDestoryQ.push_back(id);
}

SceneDesc ObjectManager::BuildSceneDesc()
{
    SceneDesc scene;
    scene.version = 1;
    scene.isOrtho = m_isOrtho;

    // Objects
    for (auto& [id, obj] : m_objMap)
    {
        scene.units.push_back(obj->GetDesc());
    }

    // Lights
    if (m_directionLight)
        scene.dirLight = m_directionLight->GetDesc();

    for (auto& pl : m_pointLights)
        scene.pointLights.push_back(pl->GetDesc());

    return scene;
}

void ObjectManager::ApplyUnitFromDesc(const std::vector<UnitDesc>& uds)
{
    if (m_objs.empty()) return;
 
    for (auto& d : uds)
    {
        Unit* o = FindObject(d.ID);
        if (!o || o->GetName() != d.name) return;

        XMFLOAT3 radRot = { XMConvertToRadians(d.transform.rotation.x), XMConvertToRadians(d.transform.rotation.y), XMConvertToRadians(d.transform.rotation.z) };

        o->SetPos(ToXM(d.transform.position));
        o->SetRot(radRot);
        o->SetScale(ToXM(d.transform.scale));
    }
}

void ObjectManager::ApplyDirLightFromDesc(const DirectionalLightDesc& dd)
{
    if (!m_directionLight) return;

    m_directionLight->SetDesc(dd);
}

void ObjectManager::ApplyPointLightsFromDesc(const std::vector<PointLightDesc>& pds)
{
    if (m_pointLights.empty()) return;

    for (auto& d : m_pointLights)
    {
        d->SetDesc(pds[d->GetDesc().id - 1]);
    }
}

void ObjectManager::CheckDedicateObjectName(std::wstring& name)
{
    int count = 0;

    for (auto& obj : m_pendingCreateQ)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    for (auto& [id, obj] : m_objMap)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    if (count)
        name += std::to_wstring(count);
}

void ObjectManager::FrameDataUpdate()
{
    // 오브젝트 매니저가 라이트를 가지고 있는데
    // 이 라이트는 씬에서 만들어서 넘겨주는?
    // 디렉션 1개만 포인트 스팟 >> 벡터
    if (m_directionLight) // 라이트가 있으면 업데이트
    {
        dirData.Lightdir = m_directionLight->GetDirFloat4Reverse();
        dirData.Lightdiff = m_directionLight->GetDiffFloat4();
        dirData.Lightamb = m_directionLight->GetAmbFloat4();
        dirData.Lightspec = m_directionLight->GetSpecFloat4();
        dirData.intensity = m_directionLight->GetIntensity();
    }

    if (!m_pointLights.empty())
    {
        int i = 0;
        for (auto& pl : m_pointLights)
        {
            plData[i].pos = pl->GetPos();
            plData[i].col = pl->GetLightColor();
            plData[i].intensity = pl->GetIntensity();
            i++;
        }
    }
}

void ObjectManager::FrameDataSubmit()
{
    // 여기서 서브밋할 예정
    auto* renderer = YunoEngine::GetRenderer();
    renderer->GetCamera().SetOrthoFlag(m_isOrtho);

    renderer->BindConstantBuffers_Camera(dirData);

    if (m_directionLight)// 라이트가 있으면 프레임 데이터 넘기기
    {
        renderer->BindConstantBuffers_Light(dirData, plData, m_pointLights.size());
    }
    
}

std::unique_ptr<MeshNode> ObjectManager::CreateMeshNode(const std::wstring& filepath, PassOption opt)
{
    return Parser::Instance().LoadFile(filepath, opt);
}
