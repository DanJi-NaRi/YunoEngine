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
    if (!m_MeshNode) return true;

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
    if (!child || !m_animator)
        return;

    if (m_animator->GetBoneCount() <= idx || 0 > idx)
        return;

    Attach(child);

    auto prev = m_ChildToIdx.find(child);
    if (prev != m_ChildToIdx.end() && prev->second != idx)
    {
        auto& prevChilds = m_BoneChilds[prev->second];
        for (auto it = prevChilds.begin(); it != prevChilds.end(); ++it)
        {
            if (*it == child)
            {
                prevChilds.erase(it);
                break;
            }
        }
    }

    auto it = m_BoneChilds.find(idx);
    if (it == m_BoneChilds.end())
    {
        m_BoneChilds[idx] = std::vector<Unit*>();
        m_BoneChilds[idx].push_back(child);
    }
    else
    {
        bool exists = false;
        for (Unit* registeredChild : it->second)
        {
            if (registeredChild == child)
            {
                exists = true;
                break;
            }
        }

        if (!exists)
            it->second.push_back(child);
    }
    m_ChildToIdx[child] = idx; //나중에 자식이 연결된 인덱스 찾기용

}

void AnimationUnit::AttachChildBone(Unit* child, const std::string& bonename)
{
    if (!child || !m_animator)
        return;

    auto idx = m_animator->FindIndex(bonename);

    if (m_animator->GetBoneCount() <= idx || 0 > idx)
        return;

    AttachChildBone(child, idx);
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

XMMATRIX AnimationUnit::GetAttachMatrixForChild(Unit* child)
{
    if (!m_animator)
        return GetWorldMatrix();

    auto it = m_ChildToIdx.find(child);
    if (it == m_ChildToIdx.end())
        return GetWorldMatrix();

    int boneIdx = it->second;

    XMMATRIX boneGlobal =
        XMLoadFloat4x4(&m_animator->GetBoneGlobalNoOffset(boneIdx));

    XMVECTOR scale;
    XMVECTOR rot;
    XMVECTOR trans;
    XMMatrixDecompose(&scale, &rot, &trans, boneGlobal);

    //XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_mScale)) *
    return boneGlobal * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_mScale)) * GetWorldMatrix();
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
