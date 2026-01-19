#pragma once
#include "pch.h"
#include "fmodPCH.h"
#include "EventHandle.h"

EventHandle::EventHandle(FMOD::Studio::EventDescription* desc)
{
    CheckFMOD(desc->createInstance(&m_Inst), "eventHandler create");
}

bool EventHandle::Valid() const
{
    return m_Inst != nullptr;
}

void EventHandle::Start()
{
    if (m_Inst) m_Inst->start();
}

void EventHandle::Stop(bool allowFadeOut, bool stopAndRelease)
{
    if (!m_Inst) return;
    m_Inst->stop(allowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
    if (stopAndRelease)
    {
        m_Inst->release();
        m_Inst = nullptr;
    }
}

void EventHandle::Release()
{
    if (!m_Inst) return;
    m_Inst->release();
    m_Inst = nullptr;
}

void EventHandle::SetParameter(FMOD_STUDIO_PARAMETER_ID id, float value, bool ignoreSeekSpeed)
{
    if (!m_Inst) return;
    m_Inst->setParameterByID(id, value, ignoreSeekSpeed);
}

void EventHandle::Set3DAttributes(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    m_3DAttributes.position = { pos.x, pos.y, pos.z };
    m_3DAttributes.velocity = { vel.x, vel.y, vel.z };
    m_3DAttributes.forward = { forward.x, forward.y, forward.z };
    m_3DAttributes.up = { up.x, up.y, up.z };

    if (!m_Inst) return;
    m_Inst->set3DAttributes(&m_3DAttributes);
}

void EventHandle::SetPaused(bool want2pause)
{
    if (!m_Inst) return;
    m_Inst->setPaused(want2pause);
}

void EventHandle::SetVolume(float volume)
{
    if (!m_Inst) return;
    m_Inst->setVolume(volume);
}
