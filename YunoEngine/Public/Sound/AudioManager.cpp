#pragma once

#include "pch.h"
#include "fmodPCH.h"

#include <vector>

#include "Bank.h"
#include "AudioCore.h"
#include "EventHandle.h"
#include "AudioQueue.h"

#include "AudioManager.h"

const std::string to_string(BankName event)
{
    std::string res;
    //switch (event)
    //{
    //case BankName::Title:
    //    res = "Title";
    //    break;
    //case BankName::Play:
    //    res = "Play";
    //    break;
    //case BankName::UI:
    //    res = "UI";
    //    break;
    //}
    return res;
}

const std::string to_string(GroupName event)
{
    return std::string();
}

const std::string to_string(VolumeType event)
{
    std::string res;
    switch (event)
    {
    case VolumeType::Master:
        res = "Master";
        break;
    case VolumeType::Music:
        res = "Music";
        break;
    case VolumeType::SFX:
        res = "SFX";
        break;
    }
    return res;
}

const std::string to_string(EventName event)
{
    std::string res;
    switch (event)
    {
    case EventName::BGM_Title:
        res = "BGM/Title";
        break;
    case EventName::BGM_Lobby:
        res = "BGM/Lobby";
        break;
    case EventName::BGM_Main:
        res = "BGM/Main";
        break;
    case EventName::UI_MouseHover:
        res = "UI/MouseHover";
        break;
    case EventName::UI_MouseClick:
        res = "UI/MouseClick";
        break;
    case EventName::UI_CountDown:
        res = "UI/CountDown";
        break;
    case EventName::UI_CointEvent:
        res = "UI/CointEvent";
        break;
    case EventName::UI_CoinToss:
        res = "UI/CoinToss";
        break;
    case EventName::UI_CardUse:
        res = "UI/CardUse";
        break;
    case EventName::UI_Win:
        res = "UI/Win";
        break;
    case EventName::UI_Lose:
        res = "UI/Lose";
        break;
    case EventName::UI_Draw:
        res = "UI/Draw";
        break;
    case EventName::UI_RoundStart:
        res = "UI/RoundStart";
        break;
    case EventName::UI_RoundOver:
        res = "UI/RoundOver";
        break;
    case EventName::UI_Emogi:
        res = "UI/Emogi";
        break;
    case EventName::UI_Move:
        res = "UI/Move";
        break;
    case EventName::PLAYER_BlasterAttack:
        res = "PLAYER/BlasterAttack";
        break;
    case EventName::PLAYER_CharkramAttack:
        res = "PLAYER/CharkramAttack";
        break;
    case EventName::PLAYER_BreacherAttack:
        res = "PLAYER/BreacherAttack";
        break;
    case EventName::PLAYER_ScytheAttack:
        res = "PLAYER/ScytheAttack";
        break;
    case EventName::PLAYER_ImpactorAttack:
        res = "PLAYER/ImpactorAttack";
        break;
    case EventName::PLAYER_CleaverAttack:
        res = "PLAYER/CleaverAttack";
        break;
    case EventName::PLAYER_StaminaBuff:
        res = "PLAYER/StaminaBuff";
        break;
    case EventName::PLAYER_PowerUpBuff:
        res = "PLAYER/PowerUpBuff";
        break;
    case EventName::PLAYER_ShieldBuff:
        res = "PLAYER/ShieldBuff";
        break;
    case EventName::PLAYER_DeBuff:
        res = "PLAYER/DeBuff";
        break;
    case EventName::PLAYER_Move:
        res = "PLAYER/Move";
        break;
    case EventName::PLAYER_Crash:
        res = "PLAYER/Crash";
        break;
    case EventName::PLAYER_Hit:
        res = "PLAYER/Hit";
        break;
    case EventName::PLAYER_Dead:
        res = "PLAYER/Dead";
        break;
    case EventName::PLAYER_TileHit:
        res = "PLAYER/TileHit";
        break;
    case EventName::PLAYER_HorizonLazer:
        res = "PLAYER/HorizonLazer";
        break;
    case EventName::PLAYER_VerticalLazer:
        res = "PLAYER/VerticalLazer";
        break;
    case EventName::PLAYER_TileCollapse:
        res = "PLAYER/TileCollapse";
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


AudioManager::~AudioManager()
{
    ClearInstList();
    m_AQ.Clear();
}

void AudioManager::Load(std::string bankName)
{
    m_Banks.push_back(bankName);
    AudioCore::Get().LoadBank(bankName);
}


void AudioManager::Unload(std::string bankName)
{
    auto it = std::find(m_Banks.begin(), m_Banks.end(), bankName);
    if (it == m_Banks.end())
    {
        std::cerr << "[AudioManager]" << "Unload : " << "The bank has never loaded before.\n";
        return;
    }
    m_Banks.erase(it);

    const auto& eventList = AudioCore::Get().GetEventList(bankName);

    for (const auto& eventName : eventList)
    {
        auto insIt = m_InstList.find(eventName);
        if (insIt == m_InstList.end()) continue;
            
        if (insIt->second.Valid())          // 객체가 재생 상태라면
        {
            insIt->second.Stop(true, true); // fadeout + release
        }
        m_InstList.erase(insIt);
    }
    
    AudioCore::Get().UnloadBank(bankName);
}

void AudioManager::Update(float dt)
{
    while (!m_AQ.Empty())
    {
        const AudioCmd& cmd = m_AQ.Pop();
        switch (cmd.type)
        {
        case AudioCmdType::LoadBank:
            Load(to_string(cmd.lb.bank));
            break;
        case AudioCmdType::UnloadBank:
            Unload(to_string(cmd.ulb.bank));
            break;


        case AudioCmdType::ListenerUpdate:
        {
            auto [x, y, z] = cmd.lu.pos;
            Listener3DUpdate({ x, y, z });
            break;
        }

        case AudioCmdType::EmitterUpdate:
        {
            auto [x, y, z] = cmd.eu.pos;
            Emitter3DUpdate(to_string(cmd.eu.event), { x, y, z });
            break;
        }

        case AudioCmdType::PlayEvent:
            PlayEvent(to_string(cmd.pe.event), cmd.pe.is3D, { cmd.pe.pos.x, cmd.pe.pos.y, cmd.pe.pos.z });
            break;
        case AudioCmdType::PlayOneShot:
            PlayOneShot(to_string(cmd.po.event));
            break;


        case AudioCmdType::StopOrRestartEvent:
            StopOrRestartEvent(to_string(cmd.sre.event), cmd.sre.is);
            break;
        case AudioCmdType::PauseOrResumeEvent:
            PauseOrResumeEvent(to_string(cmd.pre.event), cmd.pre.is);
            break;


        case AudioCmdType::SetParam:
            SetParam(to_string(cmd.sp.event), to_string(cmd.sp.param), cmd.sp.value);
            break;


        case AudioCmdType::SetGroupMute:
            SetGroupMute(to_string(cmd.sgm.group), cmd.sgm.is);
            break;
        case AudioCmdType::SetGroupPaused:
            SetGroupPaused(to_string(cmd.sgp.group), cmd.sgp.is);
            break;


        case AudioCmdType::SetUserVolume:
            SetUserVolume(to_string(cmd.suv.volumetype), cmd.suv.volume);
            break;
        }
    }
}

void AudioManager::Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    FMOD_3D_ATTRIBUTES attr = {};
    attr.position = { pos.x, pos.y, pos.z };
    attr.forward = { forward.x, forward.y, forward.z };
    attr.up = { up.x, up.y, up.z };
    attr.velocity = { vel.x, vel.y, vel.z };

    AudioCore::Get().SetListener3DAttributes(attr);
}

void AudioManager::Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    auto it = m_InstList.find(eventName);
    if (it == m_InstList.end())
    {
        std::cerr << "[AudioManager]" <<"Emitter3DUpdate : " << "Failed to find the Instance with that name.\n";
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
void AudioManager::PlayEvent(const std::string& eventName, bool is3D, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up, XMFLOAT3 vel)
{
    auto it = m_InstList.find(eventName);
    if (it != m_InstList.end())
    {
        it->second.Start();
        return;
    }

    auto desc = AudioCore::Get().GetEventDesc(eventName);
    if (desc == nullptr)
    {
        std::cerr << "해당 음원은 현재 씬에 없습니다.\n";
        return;
    }

    FMOD::Studio::EventInstance* inst = nullptr;
    FMOD_RESULT fres = desc->createInstance(&inst);
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

void AudioManager::PlayOneShot(const std::string& eventName)
{
    auto desc = AudioCore::Get().GetEventDesc(eventName);
    if (desc == nullptr)
    {
        std::cerr << "해당 음원은 현재 씬에 없습니다.\n";
        return;
    }

    FMOD::Studio::EventInstance* inst = nullptr;
    desc->createInstance(&inst);
    if (!inst) return;

    inst->start();
    inst->release();
}

void AudioManager::StopOrRestartEvent(const std::string& eventName, bool stop)
{
    auto it = m_InstList.find(eventName);
    if (it == m_InstList.end())
    {
        std::cerr << "[AudioManager]" << "StopOrRestartEvent : " << "Failed to find the instance with the eventName.\n";
        return;
    }

    if (stop)
        it->second.Stop(true, false);
    else
        it->second.Start();
}

void AudioManager::PauseOrResumeEvent(const std::string& eventName, bool paused)
{
    auto it = m_InstList.find(eventName);
    if (it == m_InstList.end())
    {
        std::cerr << "[AudioManager]" << "PauseOrResumeEvent : " << "Failed to find the instance with the eventName.\n";
        return;
    }

    it->second.SetPaused(paused);
}

void AudioManager::SetGroupMute(const std::string& groupName, bool mute)
{
    AudioCore::Get().SetBusMute(groupName, mute);
}

void AudioManager::SetGroupPaused(const std::string& groupName, bool paused)
{
    AudioCore::Get().SetBusPaused(groupName, paused);
}

void AudioManager::SetUserVolume(const std::string& volumeType, float volume)
{
    AudioCore::Get().SetVCAVolume(volumeType, volume);
}

void AudioManager::ClearInstList()
{
    for (auto& [name, inst] : m_InstList)
    {
        if (inst.Valid())
        {
            inst.Stop(true, true); // fadeout + release
        }
    }
    m_Banks.clear();
    m_InstList.clear();
    m_EventParamCache.clear();
}

void AudioManager::BuildParamCache(const std::string& eventName, FMOD::Studio::EventDescription* desc)
{
    auto it = m_EventParamCache.find(eventName);
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

void AudioManager::SetParam(const std::string& eventName, const std::string& paramName, float value)
{
    auto it = m_EventParamCache.find(eventName);
    if (it == m_EventParamCache.end())
    {
        std::cerr << "[AudioManager]" << "SetParam : " << "Failed to find the param cache with the eventName.\n";
        return;
    }
    const auto& cache = it->second;
    const auto& pcIt = cache.find(paramName);

    if (pcIt == cache.end())
    {
        std::cerr << "[AudioManager]" << "SetParam : " << "Failed to find the ParamContent with the paramName.\n";
        return;
    }

    const auto& pc = pcIt->second;
    if (pc.is3D)
        AudioCore::Get().SetGlobalParam(pc.paramID, value);
    else
    {
        auto instIt = m_InstList.find(eventName);
        if (instIt == m_InstList.end())
        {
            std::cerr << "[AudioManager]" << "SetParam : " << "Failed to find the instance with the eventName.\n";
            return;
        }
        instIt->second.SetParameter(pc.paramID, value);
    }
}
