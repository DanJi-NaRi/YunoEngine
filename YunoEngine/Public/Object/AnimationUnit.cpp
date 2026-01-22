#include "pch.h"

#include "AnimationUnit.h"
#include "Parser.h"

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
    m_MeshNode->Submit(m_mWorld);
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

void AnimationUnit::Serialize()
{
    if (m_animator)
        m_animator->Serialize();
}
