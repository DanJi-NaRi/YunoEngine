#pragma once

class EventHandle
{
public:
    EventHandle() = default;
    explicit EventHandle(FMOD::Studio::EventInstance* inst) : mInst(inst) {}
    explicit EventHandle(FMOD::Studio::EventDescription* desc) {CheckFMOD( desc->createInstance(&mInst), "eventHandler create"); }

    bool Valid() const { return mInst != nullptr; }

    void Start()
    {
        if (mInst) mInst->start();
    }

    // stopAndRelease: true면 stop 후 release까지 수행(일회성 SFX에 권장)
    void Stop(bool allowFadeOut = true, bool stopAndRelease = false)
    {
        if (!mInst) return;
        mInst->stop(allowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
        if (stopAndRelease)
        {
            mInst->release();
            mInst = nullptr;
        }
    }

    void Release()
    {
        if (!mInst) return;
        mInst->release();
        mInst = nullptr;
    }

    void SetParameter(const char* name, float value, bool ignoreSeekSpeed = false)
    {
        if (!mInst) return;
        mInst->setParameterByName(name, value, ignoreSeekSpeed);
    }

    // 3D 음원일 시 설정 꼬옥해주면 되.
    // position 바뀔 때마다 함수 호출하여 위치 정보 갱신해주기
    void Set3DAttributes(XMFLOAT3 pos, XMFLOAT3 vel = {}, XMFLOAT3 forward = {}, XMFLOAT3 up = {0, 1, 0})
    {
        m3DAttributes.position = { pos.x, pos.y, pos.z };
        m3DAttributes.velocity = { vel.x, vel.y, vel.z };
        m3DAttributes.forward = { forward.x, forward.y, forward.z };
        m3DAttributes.up = { up.x, up.y, up.z };

        if (!mInst) return;
        mInst->set3DAttributes(&m3DAttributes);
    }

    // 일시정지 혹은 재생
    void SetPaused(bool want2pause)
    {
        if (!mInst) return;
        mInst->setPaused(want2pause);
    }

    // 웬만하면 studio에서 편집(김민경에게 부탁)하거나 파라미터로 조절하기!!
    void SetVolume(float v01)
    {
        if (!mInst) return;
        mInst->setVolume(v01);
    }

private:
    FMOD_3D_ATTRIBUTES m3DAttributes = {};
    FMOD::Studio::EventInstance* mInst = nullptr;
};
