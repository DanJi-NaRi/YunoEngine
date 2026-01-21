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
