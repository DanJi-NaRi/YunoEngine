#pragma once

#include "pch.h"
#include "fmodPCH.h"

#include <vector>

#include "Bank.h"
#include "AudioSystem.h"
#include "EventHandle.h"
#include "AudioQueue.h"

#include "AudioScene.h"


AudioScene::~AudioScene() = default;

void AudioScene::Load(std::string bankName)
{
    m_BankName = bankName;
    AudioSystem::Get().LoadBank(m_BankName);
}

void AudioScene::Unload()
{
    ClearInstList();
    m_AQ.Clear();
    AudioSystem::Get().UnloadBank(m_BankName);
}

void AudioScene::Update(float dt)
{
    while (!m_AQ.Empty())
    {
        const AudioCmd& cmd = m_AQ.Pop();
        switch (cmd.type)
        {
        case AudioCmdType::PlayEvent:
            PlayEvent(to_string(cmd.pe.event), cmd.pe.is3D, cmd.pe.pos);
            break;
        case AudioCmdType::PlayOneShot:
            PlayOneShot(to_string(cmd.po.event));
            break;
        case AudioCmdType::SetParam:
            SetParam(to_string(cmd.sp.event), to_string(cmd.sp.param), cmd.sp.value);
            break;
        case AudioCmdType::ListenerUpdate:
            Listener3DUpdate(cmd.lu.pos);
            break;
        case AudioCmdType::EmitterUpdate:
            Emitter3DUpdate(to_string(cmd.eu.event), cmd.eu.pos);
            break;
        }
    }
}

void AudioScene::Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    FMOD_3D_ATTRIBUTES attr = {};
    attr.position = { pos.x, pos.y, pos.z };
    attr.forward = { forward.x, forward.y, forward.z };
    attr.up = { up.x, up.y, up.z };
    attr.velocity = { vel.x, vel.y, vel.z };

    AudioSystem::Get().SetListener3DAttributes(attr);
}

void AudioScene::Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    auto it = m_InstList.find(eventName);
    if (it == m_InstList.end())
    {
        std::cerr << "[AudioScene]" <<"Emitter3DUpdate : " << "Failed to find the Instance with that name.\n";
        return;
    }
    it->second.Set3DAttributes(pos, forward, up, vel);
}

////////////////////////////////////////////
// eventName:
//  eventGroup + "/" + event
//  eventGroup 종류: BGM, UI, ...
//  ex) BGM의 JaneDoe면 -> "BGM/JaneDoe"
// 3D음원일 경우: is3D = true, pos = 월드 상 위치, forward = 월드 상 전방벡터, up = 월드 상 상방벡터, vel = 스피드
void AudioScene::PlayEvent(const std::string& eventName, bool is3D, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    auto it = m_InstList.find(eventName);
    if (it != m_InstList.end())
    {
        it->second.Start();
        return;
    }

    auto desc = AudioSystem::Get().GetEventDesc(eventName);

    FMOD::Studio::EventInstance* inst = nullptr;
    desc->createInstance(&inst);
    if (!inst) return;
    
    EventHandle h(inst);
    h.Start();
    
    // 루프/지속형은 씬이 관리하도록 보관
    m_InstList[eventName] = h;;

    // 3D 음원일 경우, 월드 상 위치 설정
    if (is3D)
    {
        h.Set3DAttributes(pos, forward, up, vel);
    }

    BuildParamCache(eventName, desc);
}

void AudioScene::PlayOneShot(const std::string& eventName)
{
    auto desc = AudioSystem::Get().GetEventDesc(eventName);
    if (!desc) return;

    FMOD::Studio::EventInstance* inst = nullptr;
    desc->createInstance(&inst);
    if (!inst) return;

    inst->start();
    inst->release();
}

void AudioScene::ClearInstList()
{
    for (auto& [name, inst] : m_InstList)
    {
        if (inst.Valid())
        {
            inst.Stop(true, true); // fadeout + release
        }
    }
    m_InstList.clear();
    m_EventParamCache.clear();
}

void AudioScene::BuildParamCache(const std::string& eventName, FMOD::Studio::EventDescription* desc)
{
    auto it = m_EventParamCache.find("eventName");
    if (it == m_EventParamCache.end())
        return;

    std::unordered_map<std::string, ParamContent> cache;

    int paramCount = 0;
    desc->getParameterDescriptionCount(&paramCount);

    for (int i = 0; i < paramCount; ++i)
    {
        FMOD_STUDIO_PARAMETER_DESCRIPTION pd{};
        if (desc->getParameterDescriptionByIndex(i, &pd) != FMOD_OK)
            continue;

        if (!pd.name || pd.name[0] == '\0')
            continue;

        const bool isGlobal = (pd.flags & FMOD_STUDIO_PARAMETER_GLOBAL) != 0;
        cache.emplace(pd.name, ParamContent{ pd.id, isGlobal });
    }

    m_EventParamCache.emplace(eventName, std::move(cache));
}

void AudioScene::SetParam(const std::string& eventName, const std::string& paramName, float value)
{
    auto it = m_EventParamCache.find(eventName);
    if (it == m_EventParamCache.end())
    {
        std::cerr << "[TitleAudio]" << "SetParam : " << "Failed to find the param cache with the eventName.\n";
        return;
    }
    const auto& cache = it->second;
    const auto& pcIt = cache.find(paramName);

    if (pcIt == cache.end())
    {
        std::cerr << "[TitleAudio]" << "SetParam : " << "Failed to find the ParamContent with the paramName.\n";
        return;
    }

    const auto& pc = pcIt->second;
    if (pc.is3D)
        SetGlobalParam(pc.paramID, value);
    else
    {
        auto instIt = m_InstList.find(eventName);
        if (instIt == m_InstList.end())
        {
            std::cerr << "[TitleAudio]" << "SetParam : " << "Failed to find the instance with the eventName.\n";
            return;
        }
        instIt->second.SetParameter(pc.paramID, value);
    }
}

void AudioScene::SetGlobalParam(const FMOD_STUDIO_PARAMETER_ID paramID, float value)
{
    AudioSystem::Get().SetGlobalParam(paramID, value);
}
