#include "pch.h"
#include "OptionButton.h"

#include "AudioQueue.h"

bool OptionButton::HoverJoinEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseHover));
    return true;
}

bool OptionButton::LMBPressedEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseClick));
    return true;
}
