#pragma once

class EventHandle
{
public:
    EventHandle() = default;
    explicit EventHandle(FMOD::Studio::EventInstance* inst) : m_Inst(inst) {}
    explicit EventHandle(FMOD::Studio::EventDescription* desc);

    bool Valid() const;

    void Start();

    // stopAndRelease: true면 stop 후 release까지 수행(일회성 SFX에 권장)
    void Stop(bool allowFadeOut = true, bool stopAndRelease = false);

    void Release();

    void SetParameter(FMOD_STUDIO_PARAMETER_ID id, float value, bool ignoreSeekSpeed = false);

    // 3D 음원일 시 설정 꼬옥해주면 되.
    // position 바뀔 때마다 함수 호출하여 위치 정보 갱신해주기
    void Set3DAttributes(XMFLOAT3 pos, XMFLOAT3 forward = {}, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {});

    // 일시정지 혹은 재생
    void SetPaused(bool want2pause);

    // 웬만하면 studio에서 편집(김민경에게 부탁)하거나 파라미터로 조절하기!!
    void SetVolume(float volume);

private:
    FMOD_3D_ATTRIBUTES m_3DAttributes = {};
    FMOD::Studio::EventInstance* m_Inst = nullptr;
};
