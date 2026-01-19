#include "pch.h"
#include "fmodPCH.h"

#include "Bank.h"

#include "AudioSystem.h"

bool AudioSystem::Init(int maxChannels)
{
    FMOD_RESULT r;

    r = FMOD::Studio::System::create(&mStudio);
    CheckFMOD(r, "Studio::System::create");
    if (r != FMOD_OK) return false;

    r = mStudio->getCoreSystem(&mCore);
    CheckFMOD(r, "getCoreSystem");
    if (r != FMOD_OK) return false;

    // Studio init flags / Core init flags는 프로젝트에 맞게 조정
    r = mStudio->initialize(
        maxChannels,
        FMOD_STUDIO_INIT_LIVEUPDATE | FMOD_INIT_NORMAL,   // studio 툴과 연동
        FMOD_INIT_NORMAL,
        nullptr
    );
    CheckFMOD(r, "Studio::System::initialize");
    if (r != FMOD_OK) return false;
    
    r = mStudio->loadBankFile(mBH.GetBankPath("Master.strings"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mBH.mMasterStringBank);
    CheckFMOD(r, "Studio::System::Init::LoadBank");
    if (r != FMOD_OK) return false;

    r = mStudio->loadBankFile(mBH.GetBankPath("Master"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mBH.mMasterBank);
    CheckFMOD(r, "Studio::System::Init::LoadBank");
    if (r != FMOD_OK) return false;

    // 기본 3D 설정 (단위가 '미터'라면 distanceFactor=1.0)
    Set3DSettings(1.0f, 1.0f, 1.0f);
    return true;
}

void AudioSystem::Shutdown()
{
    // banks unload
    for (auto& kv : mBanks)
    {
        if (kv.second) kv.second->unload();
    }
    mBanks.clear();
    mEventDescCache.clear();
    mBusCache.clear();
    mVCACache.clear();

    if (mStudio)
    {
        mStudio->release();
        mStudio = nullptr;
        mCore = nullptr;
    }
}

void AudioSystem::Update(float dt)
{
    (void)dt;
    if (mStudio)
        mStudio->update();
}

bool AudioSystem::LoadBank(const std::string& bankName, bool loadSampleData)
{
    auto it = mBanks.find(bankName);
    if (it != mBanks.end())
        return true;

    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT r = mStudio->loadBankFile(mBH.GetBankPath(bankName), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    CheckFMOD(r, "loadBankFile");
    if (r != FMOD_OK || !bank) return false;

    if (loadSampleData)
    {
        r = bank->loadSampleData();
        CheckFMOD(r, "bank->loadSampleData");
        // 샘플 데이터 로딩 실패해도 치명적이지 않을 수 있어 정책 선택 가능
    }

    mBanks[bankName] = bank;

    // 이 bank가 포함한 event/bus/vca 목록을 기록하고 refcount 증가
    mBH.IndexBankContent(bankName, bank);
    return true;
}

void AudioSystem::UnloadBank(const std::string& bankName)
{
    auto it = mBanks.find(bankName);
    if (it == mBanks.end()) return;

    auto contentIt = mBH.mBankContents.find(bankName);
    if (contentIt == mBH.mBankContents.end()) return;
    DecRefAndEraseCaches(contentIt->second);
    mBH.mBankContents.erase(contentIt);

    it->second->unload();
    mBanks.erase(it);

    // desc/bus/vca 캐시는 “경로 기반”이라 남아있으면 무효 포인터가 될 수 있음.
    // 간단히 전체 클리어하거나, bank별로 관련 캐시만 지우는 체계를 추가함.
}

////////////////////////////////////////////
// eventName:
//  eventGroup + "/" + event
//  eventGroup 종류: BGM, UI, ...
//  ex) BGM의 JaneDoe면 -> "BGM/JaneDoe"
FMOD::Studio::EventDescription* AudioSystem::GetEventDesc(const std::string& eventName)
{
    std::string eventPath = "event:/";
    std::string totalPath = eventPath + eventName;
    auto it = mEventDescCache.find(totalPath);
    if (it != mEventDescCache.end())
        return it->second;

    FMOD::Studio::EventDescription* desc = nullptr;
    FMOD_RESULT r = mStudio->getEvent(totalPath.c_str(), &desc);
    CheckFMOD(r, "Studio::System::getEvent");
    if (r != FMOD_OK || !desc) return nullptr;

    mEventDescCache[totalPath] = desc;
    return desc;
}

FMOD::Studio::Bus* AudioSystem::GetBus(const std::string& busName)
{
    std::string busPath = "bus:/";
    std::string totalPath = busPath + busName;
    auto it = mBusCache.find(totalPath);
    if (it != mBusCache.end())
        return it->second;

    FMOD::Studio::Bus* bus = nullptr;
    FMOD_RESULT r = mStudio->getBus(totalPath.c_str(), &bus);
    CheckFMOD(r, "getBus");
    if (r != FMOD_OK || !bus) return nullptr;

    mBusCache[totalPath] = bus;
    return bus;
}

FMOD::Studio::VCA* AudioSystem::GetVCA(const std::string& vcaName)
{
    std::string vcaPath = "vca:/";
    std::string totalPath = vcaPath + vcaName;
    auto it = mVCACache.find(totalPath);
    if (it != mVCACache.end())
        return it->second;

    FMOD::Studio::VCA* vca = nullptr;
    FMOD_RESULT r = mStudio->getVCA(totalPath.c_str(), &vca);
    CheckFMOD(r, "getVCA");
    if (r != FMOD_OK || !vca) return nullptr;

    mVCACache[totalPath] = vca;
    return vca;
}

/////////////////////////////
// vcaName:
//  Master = 전체 볼륨
//  BGM / UI / Card / Player
// volume:
//  1.f : 기본값
//  0.f : 무음
void AudioSystem::SetVCAVolume(const std::string& vcaName, float volume)
{
    auto vca = GetVCA(vcaName);
    if (!vca) return;
    vca->setVolume(volume);
}

////////////////////////////////////////////
// busName:
//  Master = 전체 버스 
void AudioSystem::SetBusMute(const std::string& busName, bool mute)
{
    auto bus = GetBus(busName);
    if (!bus) return;
    bus->setMute(mute);
}

void AudioSystem::SetBusPaused(const std::string& busName, bool paused)
{
    auto bus = GetBus(busName);
    if (!bus) return;
    bus->setPaused(paused);
}

void AudioSystem::DecRefAndEraseCaches(BankContent& content)
{
    // Events
    for (const auto& name : content.events)
    {
        auto it = mBH.mEventRef.find(name);
        if (it == mBH.mEventRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            mBH.mEventRef.erase(it);
            mEventDescCache.erase(name); // 해당 Event 캐시만 제거
        }
    }

    // Buses
    for (const auto& name : content.buses)
    {
        auto it = mBH.mBusRef.find(name);
        if (it == mBH.mBusRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            mBH.mBusRef.erase(it);
            mBusCache.erase(name); // 해당 Bus 캐시만 제거
        }
    }

    // VCAs
    for (const auto& name : content.vcas)
    {
        auto it = mBH.mVcaRef.find(name);
        if (it == mBH.mVcaRef.end()) continue;

        it->second--;
        if (it->second <= 0)
        {
            mBH.mVcaRef.erase(it);
            mVCACache.erase(name); // 해당 VCA 캐시만 제거
        }
    }
}

void AudioSystem::SetListener3DAttributes(const FMOD_3D_ATTRIBUTES& attrs, int listenerIndex)
{
    if (!mStudio) return;
    mStudio->setListenerAttributes(listenerIndex, &attrs);
}

void AudioSystem::Set3DSettings(float dopplerScale, float distanceFactor, float rolloffScale)
{
    if (!mCore) return;
    mCore->set3DSettings(dopplerScale, distanceFactor, rolloffScale);
}
