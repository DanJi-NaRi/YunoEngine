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

    bool Init(int maxChannels = 1024);
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

    void DecRefAndEraseCaches(BankContent& content);    // bank unload 시, 캐시 삭제용

    // Listener (카메라/플레이어)
    void SetListener3DAttributes(
        const FMOD_3D_ATTRIBUTES& attrs,
        int listenerIndex = 0
    );

    // 3D 세팅
    void Set3DSettings(float dopplerScale, float distanceFactor, float rolloffScale);

    FMOD::Studio::System* Studio() { return mStudio; }
    FMOD::System* Core() { return mCore; }

private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

private:
    FMOD::Studio::System* mStudio = nullptr;
    FMOD::System* mCore = nullptr;

    BankHelper mBH;
    std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEventDescCache;
    std::unordered_map<std::string, FMOD::Studio::Bus*> mBusCache;
    std::unordered_map<std::string, FMOD::Studio::VCA*> mVCACache;
    
};
