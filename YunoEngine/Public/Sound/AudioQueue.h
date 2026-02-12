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
    // master bank에 통합 로드하는 걸로
};

enum class GroupName : uint8_t
{
};

enum class VolumeType : uint8_t
{
    Master,
    Music,
    SFX
};

enum class EventName : uint8_t
{
    BGM_Title,
    BGM_Lobby,
    BGM_Main,

    UI_MouseHover,
    UI_MouseClick,
    UI_CountDown,
    UI_CointEvent,
    UI_CoinToss,
    UI_CardUse,
    UI_Win,
    UI_Lose,
    UI_Draw,
    UI_RoundStart,
    UI_RoundOver,
    UI_Emogi,
    UI_Move,

    PLAYER_BlasterAttack,
    PLAYER_CharkramAttack,
    PLAYER_BreacherAttack,
    PLAYER_ScytheAttack,
    PLAYER_ImpactorAttack,
    PLAYER_CleaverAttack,
    PLAYER_StaminaBuff,
    PLAYER_PowerUpBuff,
    PLAYER_ShieldBuff,
    PLAYER_DeBuff,
    PLAYER_Move,
    PLAYER_Crash,
    PLAYER_Hit,
    PLAYER_Dead,
    PLAYER_TileHit,
    PLAYER_HorizonLazer,
    PLAYER_VerticalLazer,
    PLAYER_TileCollapse
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
            float3 pos;
        } lu;       // ListenerUpdate
        struct
        {
            EventName event;
            float3 pos;
        } eu;       // EmitterUpdate

        struct
        {
            EventName event;
            bool is3D;
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
            float volume;
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

public:
    static AudioCmd LoadBank(BankName bank);
    static AudioCmd UnLoadBank(BankName bank);
    static AudioCmd ListenerUpdate(float x, float y, float z);
    static AudioCmd EmitterUpdate(EventName event, float x, float y, float z);
    static AudioCmd PlayEvent(EventName event, bool is3D = false, float x = 0, float y = 0, float z = 0);
    static AudioCmd PlayOneShot(EventName event);
    static AudioCmd StopOrRestartEvent(EventName event, bool isStop);
    static AudioCmd PauseOrResumeEvent(EventName event, bool isPaused);
    static AudioCmd SetParam(EventName event, ParamName param, float value);
    static AudioCmd SetGroupMute(GroupName group, bool isMute);
    static AudioCmd SetGroupPaused(GroupName group, bool isPaused);
    static AudioCmd SetUserVolume(VolumeType volumeType, float volume);
};

