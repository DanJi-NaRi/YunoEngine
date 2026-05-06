#include "pch.h"

#include "AnimTest.h"

#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"
#include "UImgui.h"

//오브젝트 타입.h

namespace {
    struct AutoReg_AnimTest
    {
        AutoReg_AnimTest()
        {
            ObjectTypeRegistry::Instance().Register(L"AnimTest", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<AnimTest>(d); });
        }
    } s_reg_AnimTest;
}

AnimTest::AnimTest()
{
    unitType = L"AnimTest";
}

AnimTest::~AnimTest()
{
}

void AnimTest::SetDissolveColor(const XMFLOAT3& col)
{
    m_dissolveColor = col;
    for (auto& mesh : m_Meshs)
    {
        mesh->SetDissolveColor(m_dissolveColor);
    }
}

void AnimTest::SetNoiseTexture(const std::wstring& filepath)
{
    for (auto& mesh : m_Meshs)
    {
        mesh->SetTexture(TextureUse::Custom, filepath);
    }
}

bool AnimTest::Update(float dTime)
{
    AnimationUnit::Update(dTime);

    if (m_dissolveAmount >= 0.0f && m_dissolveAmount <= 0.2f)
    {
        m_dissolveWidth = std::min(0.2f, m_dissolveAmount);
        for (auto& mesh : m_Meshs)
        {
            mesh->SetDissolveWidth(m_dissolveWidth);
        }
    }

    /*if (m_dissolveAmount <= 0.2f && m_dissolveWidth > 0.0f)
    {
        m_dissolveWidth = std::max(0.0f, m_dissolveAmount);
        for (auto& mesh : m_Meshs)
        {
            mesh->SetDissolveWidth(m_dissolveWidth);
        }
    }*/

    return true;
}

void AnimTest::UpdateDissolve(float dt)
{
    if (m_dissolveAmount >= 0.0f && m_dissolveAmount <= 0.2f)
    {
        m_dissolveWidth = std::min(0.2f, m_dissolveAmount);
        for (auto& mesh : m_Meshs)
        {
            mesh->SetDissolveWidth(m_dissolveWidth);
        }
    }
}

#ifdef _DEBUG
void AnimTest::Serialize()
{
    AnimationUnit::Serialize();

    if (UI::CollapsingHeader("Dissolve"))
    {
        if(UI::DragFloat3Editable("EdgeColor", &m_dissolveColor.x, 0.01f, 0.0f, 1.0f))
        {
            for (auto& mesh : m_Meshs)
            {
                mesh->SetDissolveColor(m_dissolveColor);
            }
        }

        if (UI::DragFloat("Amount", &m_dissolveAmount, 0.01f, 0.0f, 1.0f, "%.3f"))
        {
            for (auto& mesh : m_Meshs)
            {
                mesh->SetDissolveAmount(m_dissolveAmount);
            }
        }
    }
}
#endif
