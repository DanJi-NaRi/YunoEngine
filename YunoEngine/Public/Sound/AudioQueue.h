#pragma once
#include <queue>

enum class AudioCmdType
{
    PlayEvent,      // 루프 음원 재생 시
    PlayOneShot,    // 단일성 음원 재생 시
    SetParam,       // 루프 음원의 파라미터 조절하고 싶을 때

    ListenerUpdate, // 청자의 위치 갱신
    EmitterUpdate   // 3D음원 위치 갱신
};

enum class SceneType : uint8_t
{
    Title,
    Play,
    UI
};

enum class EventName : uint32_t
{
    BGM_Playlist,
    UI_Click
};
const std::string to_string(EventName event);

enum class ParamName : uint32_t
{
    Health
};
const std::string to_string(ParamName param);

struct AudioCmd
{
    AudioCmdType type;
    union
    {
        struct
        {
            SceneType scene;
            EventName event;
            bool is3D;
            XMFLOAT3 pos;
        } pe;       // PlayEvent
        struct
        {
            SceneType scene;
            EventName event;
            XMFLOAT3 pos;
        } eu;       // EmitterUpdate
        struct
        {
            SceneType scene;
            EventName event;
        } po;       // PlayOneShot
        struct
        {
            SceneType scene;
            EventName event;
            ParamName param;
            float value;
        } sp;       // SetParam
        struct
        {
            XMFLOAT3 pos;
        } lu;       // ListenerUpdate
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

