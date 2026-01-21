#include "pch.h"

#include "AudioQueue.h"

std::queue<AudioCmd> AudioQ::AudioQue;

void AudioQ::Insert(AudioCmd cmd)
{
    AudioQue.push(cmd);
}

bool AudioQ::Empty()
{
    return AudioQue.empty();
}

const AudioCmd& AudioQ::Pop()
{
    const AudioCmd& cmd = AudioQue.front();
    AudioQue.pop();
    return cmd;
}

void AudioQ::Clear()
{
    while(!AudioQue.empty())
        AudioQue.pop();
}

static const AudioCmd LoadBank(BankName bank)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd UnLoadBank(BankName bank)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd ListenerUpdate(float x, float y, float z)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd EmitterUpdate(EventName event, float x, float y, float z)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd PlayEvent(EventName event, bool is3D = false, float x = 0, float y = 0, float z = 0)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd PlayOneShot(EventName event)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd StopOrRestartEvent(EventName event, bool isStop)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd PauseOrResumeEvent(EventName event, bool isPaused)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd SetParam(EventName event, ParamName param, float value)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd SetGroupMute(GroupName group, bool isMute)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd SetGroupPaused(GroupName group, bool isPaused)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
static const AudioCmd SetUserVolume(VolumeType volumeType, float volume);
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
}
