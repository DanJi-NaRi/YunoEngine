#pragma once

class IAudioManager
{
public:
    virtual ~IAudioManager() = default;
    virtual void Update(float dt) = 0;

protected:

    virtual void Load(std::string bankName) = 0;
    virtual void Unload(std::string bankName) = 0;

    // --- 음원 객체 조절 ---

    virtual void Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;
    virtual void Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;

    virtual void PlayEvent(const std::string& eventName, bool is3D = false, XMFLOAT3 pos = {}, XMFLOAT3 forward = { 0, 0, 1 }, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;
    virtual void PlayOneShot(const std::string& eventName) = 0;

    virtual void StopOrRestartEvent(const std::string& eventName, bool stop) = 0;
    virtual void PauseOrResumeEvent(const std::string& eventName, bool paused) = 0;
    
    virtual void SetParam(const std::string& eventName, const std::string& paramName, float value) = 0;
    
    // --- 음원 묶음(그룹) 단위 상태 조절 ---

    virtual void SetGroupMute(const std::string& groupName, bool mute) = 0;
    virtual void SetGroupPaused(const std::string& groupName, bool paused) = 0;

    // --- 플레이어 음량 조절 ---

    virtual void SetUserVolume(const std::string& volumeType, float volume) = 0;

};

