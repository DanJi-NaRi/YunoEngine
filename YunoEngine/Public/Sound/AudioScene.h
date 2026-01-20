#pragma once
#include "IAudioScene.h"

struct ParamContent
{
    FMOD_STUDIO_PARAMETER_ID paramID;
    bool is3D = false;
};
// Base AudioScene
class AudioScene : public IAudioScene
{
public:
    virtual ~AudioScene();                // cpp에서 default

    // 씬 진입 시
    void Load(std::string bankName) override;

    // 씬 떠날 시
    void Unload() override;

    void Update(float dt) override;

    // 3D음원이 있다면 리스너 업데이트 필수!
    void Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward = { 0, 0, 1 }, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) override;
    // 3D음원의 위치가 바뀌었다면? 업데이트 필수!
    void Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward = { 0, 0, 1 }, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) override;

    // 씬 내내 들고 관리할 음원.(특히 루프형)
    // 2D음원 재생 시, eventName만 기입.
    // 3D음원 재생 시, eventName, is3D, pos, forward 값 전달 모두 필요
    void PlayEvent(const std::string& eventName, bool is3D = false, XMFLOAT3 pos = {}, XMFLOAT3 forward = { 0, 0, 1 }, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {}) override;

    // 한 번 재생하고 나면 사라질 음원
    void PlayOneShot(const std::string& eventName) override;


    void BuildParamCache(const std::string& eventName, FMOD::Studio::EventDescription* desc);

    // 파라미터 조절 함수
    void SetParam(const std::string& eventName, const std::string& paramName, float value) override;
    // SetParam 내부에서 전역 파라미터일 경우 아래 함수 호출
    void SetGlobalParam(const FMOD_STUDIO_PARAMETER_ID paramName, float value);

    // 씬에서 만든 인스턴스 정리
    void ClearInstList();

protected:
    std::string m_BankName;
    std::unordered_map<std::string, EventHandle> m_InstList; // 재생 음원들
    std::unordered_map<std::string, std::unordered_map<std::string, ParamContent>> m_EventParamCache; // key: 이벤트 이름, value: 파라미터 이름들

    AudioQ m_AQ;
};

