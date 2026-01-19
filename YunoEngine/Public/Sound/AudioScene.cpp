#pragma once

#include "pch.h"
#include "fmodPCH.h"

#include <vector>

#include "Bank.h"
#include "AudioSystem.h"
#include "EventHandle.h"
#include "AudioScene.h"

void AudioScene::Load(std::string bankName)
{
    m_BankName = bankName;
    AudioSystem::Get().LoadBank(m_BankName);
}

void AudioScene::Unload()
{
    ClearInstList();
    AudioSystem::Get().UnloadBank(m_BankName);
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

    // 해당 이벤트의 파라미터 정보 얻어오기
    int paramCount = 0;
    desc->getParameterDescriptionCount(&paramCount);
    if (paramCount > 0)
    {
        for (int i = 0; i < paramCount; i++)
        {
            FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc = {};
            desc->getParameterDescriptionByIndex(i, &paramDesc);
            if (paramDesc.flags == FMOD_STUDIO_PARAMETER_GLOBAL) return;    // 오로지 로컬 파라미터만 관리
            if (paramDesc.name == 0) return;
            ParamContent pc = { eventName, paramDesc.id };
            m_ParamList[paramDesc.name] = pc;
        }
    }

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
    m_ParamList.clear();
    //m_EventParameterNames.clear();
}

void TitleAudio::SetInstParam(const std::string& paramName, float value)
{
    auto it = m_ParamList.find(paramName);
    if (it == m_ParamList.end())
    {
        std::cerr << "[TitleAudio]" << "SetParam : " << "Failed to find the paramID with that name.\n";
        return;
    }
    const auto& pc = it->second;
    m_InstList[pc.eventName].SetParameter(pc.paramID, value, true);
}

void TitleAudio::SetGlobalParam(const std::string& paramName, float value)
{
    AudioSystem::Get().SetGlobalParam(paramName, value);
}
