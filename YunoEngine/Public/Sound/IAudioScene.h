#pragma once

class IAudioScene
{
public:
    virtual void Load(std::string bankName) = 0;
    virtual void Unload() = 0;
    virtual void Update(float dt) = 0;

    virtual void Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;
    virtual void Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;

    virtual void PlayEvent(const std::string& eventName, bool is3D = false, XMFLOAT3 pos = {}, XMFLOAT3 forward = { 0, 0, 1 }, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) = 0;
    virtual void PlayOneShot(const std::string& eventName) = 0;
    
    virtual void SetParam(const std::string& eventName, const std::string& paramName, float value) = 0;

    virtual ~IAudioScene() = default;
};
