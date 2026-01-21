#pragma once
#include <queue>

enum class AudioCmdType : uint8_t
{
    LoadBank,
    UnloadBank,

    ListenerUpdate, // 청자의 위치 갱신
    EmitterUpdate,   // 3D음원 위치 갱신

    PlayEvent,      // 루프 음원 재생 시
    PlayOneShot,    // 단일성 음원 재생 시

    StopOrRestartEvent,
    PauseOrResumeEvent,

    SetParam,       // 루프 음원의 파라미터 조절하고 싶을 때

    SetGroupMute,
    SetGroupPaused,

    SetUserVolume
};

enum class BankName : uint8_t
{
    Title,
    Play,
    UI
};

enum class GroupName : uint8_t
{
};

enum class VolumeType : uint8_t
{
};

enum class EventName : uint8_t
{
    BGM_Playlist,
    UI_Click
};

enum class ParamName : uint8_t
{
    Health
};

struct float3
{
    float x, y, z;
};


struct AudioCmd
{
    AudioCmdType type;

    union
    {
        struct
        {
            BankName bank;
        } lb, ulb;   // LoadBank, UnloadBank

        struct
        {
            XMFLOAT3 pos;
        } lu;       // ListenerUpdate
        struct
        {
            EventName event;
            XMFLOAT3 pos;
        } eu;       // EmitterUpdate

        struct
        {
            EventName event;
            bool is3D = false;
            float3 pos;
        } pe;       // PlayEvent
        struct
        {
            EventName event;
        } po;       // PlayOneShot

        struct
        {
            EventName event;
            bool is;// isStop            , isPaused   
        } sre, pre; // StopOrRestartEvent, PauseOrResumeEvent

        struct
        {
            EventName event;
            ParamName param;
            float value;
        } sp;       // SetParam

        struct
        {
            GroupName group;
            bool is;// isMute      , isPaused
        } sgm, sgp; // SetGroupMute, SetGroupPaused

        struct
        {
            VolumeType volumetype;
            float value;
        } suv;      // SetUserVolume

    };
 
};


class AudioQ
{
private:
    static std::queue<AudioCmd> AudioQue;
public:
    static void Insert(AudioCmd cmd);
    bool Empty();
    const AudioCmd& Pop();

    // 씬 전환 시, AudioManager에서 호출
    void Clear();
};


static const AudioCmd LoadBank(BankName bank);
static const AudioCmd UnLoadBank(BankName bank);
static const AudioCmd ListenerUpdate(float x, float y, float z);
static const AudioCmd EmitterUpdate(EventName event, float x, float y, float z);
static const AudioCmd PlayEvent(EventName event, bool is3D = false, float x = 0, float y = 0, float z = 0);
static const AudioCmd PlayOneShot(EventName event);
static const AudioCmd StopOrRestartEvent(EventName event, bool isStop);
static const AudioCmd PauseOrResumeEvent(EventName event, bool isPaused);
static const AudioCmd SetParam(EventName event, ParamName param, float value);
static const AudioCmd SetGroupMute(GroupName group, bool isMute);
static const AudioCmd SetGroupPaused(GroupName group, bool isPaused);
static const AudioCmd SetUserVolume(VolumeType volumeType, float volume);
