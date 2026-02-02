#include "pch.h"

#include "AnimationUnit.h"
#include "Parser.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

namespace {
    struct AutoReg_AnimUnit
    {
        AutoReg_AnimUnit()
        {
            ObjectTypeRegistry::Instance().Register(L"AnimationUnit", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<AnimationUnit>(d); });
        }
    } s_reg_AnimUnit;
}

AnimationUnit::AnimationUnit()
{
    unitType = L"AnimationUnit";
}

void AnimationUnit::AnimationUpdate(float dTime)
{
    if(m_animator)
        m_animator->Update(dTime);
}

bool AnimationUnit::Update(float dTime)
{
    AnimationUpdate(dTime);

    Unit::Update(dTime);

    return true;
}

bool AnimationUnit::Submit(float dTime)
{
    m_MeshNode->Submit(m_mWorld, m_vPos);
    if (m_animator)
        m_MeshNode->AnimSubmit(m_animator->GetBoneTMs());

    LastSubmit(dTime);

    return true;
}

void AnimationUnit::AddAnimationClip(const std::string& name, const std::wstring& filepath)
{
    if (m_animator)
    {
        m_animator->AddAnimationFromFile(name, filepath);
    }
    else
        m_animator = Parser::Instance().LoadAnimatorFromFile(name, filepath);
}

void AnimationUnit::AttachChildBone(Unit* child, int idx)
{
    if (m_animator->GetBoneCount() >= idx || 0 > idx)
        return;

    Attach(child);

    auto it = m_BoneChilds.find(idx);
    if (it == m_BoneChilds.end())
    {
        std::vector<Unit*> childs;
        
        m_BoneChilds[idx] = childs;
        m_BoneChilds[idx].push_back(child); 
        m_ChildToIdx.emplace(child, idx); //나중에 자식이 연결된 인덱스 찾기용
    }
    else
    {
        it->second.push_back(child);
        m_ChildToIdx.emplace(child, idx);
    }
}

void AnimationUnit::AttachChildBone(Unit* child, const std::string& bonename)
{
    auto idx = m_animator->FindIndex(bonename);

    if (m_animator->GetBoneCount() >= idx || 0 > idx)
        return;

    Attach(child);

    auto it = m_BoneChilds.find(idx);
    if (it == m_BoneChilds.end())
    {
        std::vector<Unit*> childs;

        m_BoneChilds[idx] = childs;
        m_BoneChilds[idx].push_back(child);
        m_ChildToIdx.emplace(child, idx);
    }
    else
    {
        it->second.push_back(child);
        m_ChildToIdx.emplace(child, idx);
    }
}

void AnimationUnit::DettachChild(uint32_t id)
{
    auto it = m_Childs.find(id);
    if (it == m_Childs.end())
        return;

    auto it2 = m_ChildToIdx.find(it->second);
    if (it2 != m_ChildToIdx.end())
    {
        for (auto it3 = m_BoneChilds[it2->second].begin(); it3 != m_BoneChilds[it2->second].end(); it3++)
        {
            if (*it3 == it->second)
            {
                m_BoneChilds[it2->second].erase(it3);
                break;
            }
        }
        m_ChildToIdx.erase(it2);
    }

    m_Childs.erase(it);
}

void AnimationUnit::ClearChild()
{
    Unit::ClearChild();

    m_BoneChilds.clear();
    m_ChildToIdx.clear();
}

void AnimationUnit::SetLoop(const std::string& name, bool isLoop)
{
    m_animator->SetLoop(name, isLoop);
}

void AnimationUnit::SetLoop(UINT id, bool isLoop)
{
    m_animator->SetLoop(id, isLoop);
}

#ifdef _DEBUG
void AnimationUnit::Serialize()
{
    Unit::Serialize();

    if (m_animator)
        m_animator->Serialize();
}
#endif
