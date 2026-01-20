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

const std::string to_string(EventName event)
{
    std::string res;
    switch (event)
    {
    case EventName::BGM_Playlist:
        res = "BGM/Playlist";
        break;
    case EventName::UI_Click:
        res = "UI/Click";
        break;
    }
    return res;
}

const std::string to_string(ParamName param)
{
    std::string res;
    switch (param)
    {
    case ParamName::Health:
        res = "Health";
        break;
    }
    return res;
}
