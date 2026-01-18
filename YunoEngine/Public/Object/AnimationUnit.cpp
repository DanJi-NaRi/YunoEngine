#include "pch.h"

#include "AnimationUnit.h"

void AnimationUnit::AnimationUpdate(float dTime)
{
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
    m_MeshNode->AnimSubmit(m_animator->GetBoneTMs());

    LastSubmit(dTime);

    return true;
}
