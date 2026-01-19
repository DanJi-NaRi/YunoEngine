#pragma once

#include "pch.h"
#include "fmodPCH.h"

#include <vector>

#include "Bank.h"
#include "AudioSystem.h"
#include "EventHandle.h"
#include "AudioScene.h"

////////////////////////////////////////////
// eventName:
//  eventGroup + "/" + event
//  eventGroup 종류: BGM, UI, ...
//  ex) BGM의 JaneDoe면 -> "BGM/JaneDoe"
// *** 루프형 음원일 경우, oneShot = true(X) ***
void AudioScene::PlayEvent(const std::string& eventName, bool is3D, XMFLOAT3 pos, XMFLOAT3 dir)
{
    auto it = mInstList.find(eventName);
    if (it != mInstList.end())
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
    mInstList[eventName] = h;;
    if (is3D)
    {

    }
    else
    {

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
            mParamList[paramDesc.name] = paramDesc.id;
        }
    }

}

void AudioScene::ClearInstList()
{
    for (auto& [name, inst] : mInstList)
    {
        if (inst.Valid())
        {
            inst.Stop(true, true); // fadeout + release
        }
    }
    mInstList.clear();
    mParamList.clear();
}
