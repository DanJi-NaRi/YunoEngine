#include "pch.h"
#include "fmodPCH.h"

#include "Bank.h"

#include "AudioCore.h"

bool AudioCore::Init(int maxChannels)
{
    FMOD_RESULT r;

    r = FMOD::Studio::System::create(&m_Studio);
    CheckFMOD(r, "Studio::System::create");
    if (r != FMOD_OK) return false;

    r = m_Studio->getCoreSystem(&m_Core);
    CheckFMOD(r, "getCoreSystem");
    if (r != FMOD_OK) return false;

    // Studio init flags / Core init flags는 프로젝트에 맞게 조정
    r = m_Studio->initialize(
        maxChannels,
        FMOD_STUDIO_INIT_LIVEUPDATE | FMOD_INIT_NORMAL,   // studio 툴과 연동
        FMOD_INIT_NORMAL,
        nullptr
    );
    CheckFMOD(r, "Studio::System::initialize");
    if (r != FMOD_OK) return false;
    
    r = m_Studio->loadBankFile(m_BH.GetBankPath("Master.strings"), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_BH.m_MasterStringBank);
    CheckFMOD(r, "Studio::System::Init::LoadBank");
    if (r != FMOD_OK) return false;

    r = m_Studio->loadBankFile(m_BH.GetBankPath("Master"), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_BH.m_MasterBank);
    CheckFMOD(r, "Studio::System::Init::LoadBank");
    if (r != FMOD_OK) return false;

    // 기본 3D 설정 (단위가 '미터'라면 distanceFactor=1.0)
    Set3DSettings(1.0f, 1.0f, 1.0f);
    return true;
}

void AudioCore::Shutdown()
{
    // banks unload
    for (auto& kv : m_Banks)
    {
        if (kv.second) kv.second->unload();
    }
    m_Banks.clear();
    m_EventDescList.clear();
    m_BusList.clear();
    m_VCAList.clear();

    if (m_Studio)
    {
        m_Studio->release();
        m_Studio = nullptr;
        m_Core = nullptr;
    }
}

void AudioCore::Update(float dt)
{
    (void)dt;
    if (m_Studio)
        m_Studio->update();
}

bool AudioCore::LoadBank(const std::string& bankName, bool loadSampleData)
{
    auto it = m_Banks.find(bankName);
    if (it != m_Banks.end())
        return true;

    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT r = m_Studio->loadBankFile(m_BH.GetBankPath(bankName), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    CheckFMOD(r, "loadBankFile");
    if (r != FMOD_OK || !bank) return false;

    if (loadSampleData)
    {
        r = bank->loadSampleData();
        CheckFMOD(r, "bank->loadSampleData");
        // 샘플 데이터 로딩 실패해도 치명적이지 않을 수 있어 정책 선택 가능
    }

    m_Banks[bankName] = bank;

    // 이 bank가 포함한 event/bus/vca 목록을 기록하고 refcount 증가
    m_BH.IndexBankContent(bankName, bank);

    return true;
}

void AudioCore::UnloadBank(const std::string& bankName)
{
    auto it = m_Banks.find(bankName);
    if (it == m_Banks.end()) return;

    auto contentIt = m_BH.m_BankContents.find(bankName);
    if (contentIt == m_BH.m_BankContents.end()) return;
    DecRefAndEraseCaches(contentIt->second);
    m_BH.m_BankContents.erase(contentIt);

    it->second->unload();
    m_Banks.erase(it);

    // desc/bus/vca 캐시는 “경로 기반”이라 남아있으면 무효 포인터가 될 수 있음.
    // 간단히 전체 클리어하거나, bank별로 관련 캐시만 지우는 체계를 추가함.
}

////////////////////////////////////////////
// eventName:
//  eventGroup + "/" + event
//  eventGroup 종류: BGM, UI, ...
//  ex) BGM의 JaneDoe면 -> "BGM/JaneDoe"
FMOD::Studio::EventDescription* AudioCore::GetEventDesc(const std::string& eventName)
{
    auto it = m_EventDescList.find(eventName);
    if (it != m_EventDescList.end())
        return it->second;

    FMOD::Studio::EventDescription* desc = nullptr;
    std::string eventPath = "event:/";
    std::string totalPath = eventPath + eventName;
    FMOD_RESULT r = m_Studio->getEvent(totalPath.c_str(), &desc);
    CheckFMOD(r, "Studio::System::getEvent");
    if (r != FMOD_OK || !desc) return nullptr;
    
    m_EventDescList[eventName] = desc;
    return desc;
}

FMOD::Studio::Bus* AudioCore::GetBus(const std::string& busName)
{
    auto it = m_BusList.find(busName);
    if (it != m_BusList.end())
        return it->second;

    FMOD::Studio::Bus* bus = nullptr;
    std::string busPath = "bus:/";
    std::string totalPath = busPath + busName;
    FMOD_RESULT r = m_Studio->getBus(totalPath.c_str(), &bus);
    CheckFMOD(r, "getBus");
    if (r != FMOD_OK || !bus) return nullptr;

    m_BusList[busName] = bus;
    return bus;
}

FMOD::Studio::VCA* AudioCore::GetVCA(const std::string& vcaName)
{
    auto it = m_VCAList.find(vcaName);
    if (it != m_VCAList.end())
        return it->second;

    FMOD::Studio::VCA* vca = nullptr;
    std::string vcaPath = "vca:/";
    std::string totalPath = vcaPath + vcaName;
    FMOD_RESULT r = m_Studio->getVCA(totalPath.c_str(), &vca);
    CheckFMOD(r, "getVCA");
    if (r != FMOD_OK || !vca) return nullptr;

    m_VCAList[vcaName] = vca;
    return vca;
}

/////////////////////////////
// vcaName:
//  Master = 전체 볼륨
//  BGM / UI / Card / Player
// volume:
//  1.f : 기본값
//  0.f : 무음
void AudioCore::SetVCAVolume(const std::string& vcaName, float volume)
{
    auto vca = GetVCA(vcaName);
    if (!vca) return;
    vca->setVolume(volume);
}

////////////////////////////////////////////
// busName:
//  Master = 전체 버스 
void AudioCore::SetBusMute(const std::string& busName, bool mute)
{
    auto bus = GetBus(busName);
    if (!bus) return;
    bus->setMute(mute);
}

void AudioCore::SetBusPaused(const std::string& busName, bool paused)
{
    auto bus = GetBus(busName);
    if (!bus) return;
    bus->setPaused(paused);
}

void AudioCore::DecRefAndEraseCaches(BankContent& content)
{
    // Events
    for (const auto& name : content.events)
    {
        auto it = m_BH.m_EventRef.find(name);
        if (it == m_BH.m_EventRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            m_BH.m_EventRef.erase(it);
            m_EventDescList.erase(name); // 해당 Event 캐시만 제거
        }
    }

    // Buses
    for (const auto& name : content.buses)
    {
        auto it = m_BH.m_BusRef.find(name);
        if (it == m_BH.m_BusRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            m_BH.m_BusRef.erase(it);
            m_BusList.erase(name); // 해당 Bus 캐시만 제거
        }
    }

    // VCAs
    for (const auto& name : content.vcas)
    {
        auto it = m_BH.m_VcaRef.find(name);
        if (it == m_BH.m_VcaRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            m_BH.m_VcaRef.erase(it);
            m_VCAList.erase(name); // 해당 VCA 캐시만 제거
        }
    }
}

void AudioCore::SetListener3DAttributes(const FMOD_3D_ATTRIBUTES& attrs, int listenerIndex)
{
    if (!m_Studio) return;
    m_Studio->setListenerAttributes(listenerIndex, &attrs);
}

void AudioCore::SetListener3DAttributes(const XMFLOAT3& pos)
{
    FMOD_3D_ATTRIBUTES attrs = {};
    attrs.position = { pos.x, pos.y, pos.z };
    attrs.forward = { 0, 0, 1 };
    attrs.up = { 0, 1, 0 };
    attrs.velocity = { 0, 0, 0 };

    if (!m_Studio) return;
    m_Studio->setListenerAttributes(0, &attrs);
}

void AudioCore::SetGlobalParam(const FMOD_STUDIO_PARAMETER_ID paramID, float value)
{
    m_Studio->setParameterByID(paramID, value, true);
}

void AudioCore::Set3DSettings(float dopplerScale, float distanceFactor, float rolloffScale)
{
    if (!m_Core) return;
    m_Core->set3DSettings(dopplerScale, distanceFactor, rolloffScale);
}

const std::vector<std::string>& AudioCore::GetEventList(const std::string bankName)
{
    auto it = m_BH.m_BankContents.find(bankName);
    if (it == m_BH.m_BankContents.end())
    {
        std::cerr << "[AudioCore]" << "GetEventList : " << "Failed to find the eventList with that bankName.\n";
    }

    return it->second.events;
}
