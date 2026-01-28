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

#ifdef _DEBUG
void AnimationUnit::Serialize()
{
    Unit::Serialize();

    if (m_animator)
        m_animator->Serialize();
}
#endif
