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
            if (paramDesc.name == 0) return;
            m_ParamList[paramDesc.name] = paramDesc.id;
        }
    }

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
    m_EventParameterNames.clear();
}
