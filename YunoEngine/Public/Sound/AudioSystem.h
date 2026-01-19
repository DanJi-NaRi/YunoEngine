#pragma once
#include <unordered_map>
#include <vector>

class AudioSystem
{
public:
    static AudioSystem& Get()
    {
        static AudioSystem s;
        return s;
    }

    // 게임 시작 시 엔진과 함께 꼬옥 초기화 해주기
    bool Init(int maxChannels = 1024);
    // 게임 종료 시 꼬옥 호출해주기
    void Shutdown();

    void Update(float dt);

    // Bank
    bool LoadBank(const std::string& bankName, bool loadSampleData = true);
    void UnloadBank(const std::string& bankName);

    // Events
    FMOD::Studio::EventDescription* GetEventDesc(const std::string& eventName);

    // Bus / VCA
    FMOD::Studio::Bus* GetBus(const std::string& busName);
    FMOD::Studio::VCA* GetVCA(const std::string& vcaName);

    // 볼륨 사용자화 용(for Players)
    void SetVCAVolume(const std::string& vcaName, float volume);    // volume: 0 ~ 1
    
    // 소리 상태 설정 용(for Coders)
    void SetBusMute(const std::string& busName, bool mute);
    void SetBusPaused(const std::string& busName, bool paused);

    // bank unload 시, 캐시 삭제용
    void DecRefAndEraseCaches(BankContent& content);

    // Listener (카메라/플레이어)
    void SetListener3DAttributes(
        const FMOD_3D_ATTRIBUTES& attrs,
        int listenerIndex = 0
    );

    // bank 단위로 생성 소멸되므로 load()시 내부에서 호출
    void CacheGlobalParam();
    void SetGlobalParam(const std::string& paramName, float value);

    // 3D 세팅
    void Set3DSettings(float dopplerScale, float distanceFactor, float rolloffScale);

    FMOD::Studio::System* Studio() { return m_Studio; }
    FMOD::System* Core() { return m_Core; }

private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

private:
    FMOD::Studio::System* m_Studio = nullptr;
    FMOD::System* m_Core = nullptr;

    BankHelper m_BH;
    std::unordered_map<std::string, FMOD::Studio::Bank*> m_Banks;
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> m_EventDescList;
    std::unordered_map<std::string, FMOD::Studio::Bus*> m_BusList;
    std::unordered_map<std::string, FMOD::Studio::VCA*> m_VCAList;
    std::unordered_map<std::string, FMOD_STUDIO_PARAMETER_ID> m_GlobalParam;
};
