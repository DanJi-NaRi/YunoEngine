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


AudioCmd AudioQ::LoadBank(BankName bank)
{
    AudioCmd cmd{ AudioCmdType::LoadBank };
    cmd.lb.bank = bank;
    return cmd;
}
AudioCmd AudioQ::UnLoadBank(BankName bank)
{
    AudioCmd cmd{ AudioCmdType::UnloadBank };
    cmd.ulb.bank = bank;
    return cmd;
}
AudioCmd AudioQ::ListenerUpdate(float x, float y, float z)
{
    AudioCmd cmd{ AudioCmdType::ListenerUpdate };
    cmd.lu.pos = { x, y, z };
    return cmd;
}
AudioCmd AudioQ::EmitterUpdate(EventName event, float x, float y, float z)
{
    AudioCmd cmd{ AudioCmdType::EmitterUpdate };
    cmd.eu.event = event;
    cmd.eu.pos = { x, y, z };
    return cmd;
}
AudioCmd AudioQ::PlayEvent(EventName event, bool is3D, float x, float y, float z)
{
    AudioCmd cmd{ AudioCmdType::PlayEvent };
    cmd.pe.event = event;
    cmd.pe.is3D = is3D;
    cmd.pe.pos = { x, y, z };
    return cmd;
}
AudioCmd AudioQ::PlayOneShot(EventName event)
{
    AudioCmd cmd{ AudioCmdType::PlayOneShot };
    cmd.po.event = event;
    return cmd;
}
AudioCmd AudioQ::StopOrRestartEvent(EventName event, bool isStop)
{
    AudioCmd cmd{ AudioCmdType::StopOrRestartEvent };
    cmd.sre.event = event;
    cmd.sre.is = isStop;
    return cmd;
}
AudioCmd AudioQ::PauseOrResumeEvent(EventName event, bool isPaused)
{
    AudioCmd cmd{ AudioCmdType::PauseOrResumeEvent };
    cmd.pre.event = event;
    cmd.pre.is = isPaused;
    return cmd;
}
AudioCmd AudioQ::SetParam(EventName event, ParamName param, float value)
{
    AudioCmd cmd{ AudioCmdType::SetParam };
    cmd.sp.event = event;
    cmd.sp.param = param;
    cmd.sp.value = value;
    return cmd;
}
AudioCmd AudioQ::SetGroupMute(GroupName group, bool isMute)
{
    AudioCmd cmd{ AudioCmdType::SetGroupMute };
    cmd.sgm.group = group;
    cmd.sgm.is = isMute;
    return cmd;
}
AudioCmd AudioQ::SetGroupPaused(GroupName group, bool isPaused)
{
    AudioCmd cmd{ AudioCmdType::SetGroupPaused };
    cmd.sgp.group = group;
    cmd.sgp.is = isPaused;
    return cmd;
}
AudioCmd AudioQ::SetUserVolume(VolumeType volumeType, float volume)
{
    AudioCmd cmd{ AudioCmdType::SetUserVolume };
    cmd.suv.volumetype = volumeType;
    cmd.suv.volume = volume;
    return cmd;
}
