#include "pch.h"
#include "AudioScenePCH.h"
// 인클루드 순서
// 1.본인 씬 헤더
#include "SceneBase.h"

#include "IRenderer.h"
#include "ObjectManager.h"


SceneBase::SceneBase() = default;
SceneBase::~SceneBase() = default;

//AudioScene::AudioScene() {};
//AudioScene::~AudioScene() {};

// 생성
bool SceneBase::OnCreate()
{
    //std::cout << "[SceneBase] OnCreate\n";
    m_objectManager = std::make_unique<ObjectManager>();
    if (!m_objectManager)
        return false;

    if (!m_objectManager->Init())
        return false;

    m_audioScene = std::make_unique<AudioScene>();
    if (!m_audioScene)
        return false;

    return OnCreateScene();
}

// 삭제
void SceneBase::OnDestroy()
{
    //std::cout << "[SceneBase] OnDestroy\n";

    OnDestroyScene();

    if (m_objectManager)
    {
        m_objectManager->Clear();
        m_objectManager.reset();
    }
}

// 진입
void SceneBase::OnEnter()
{
    //std::cout << "[SceneBase] OnEnter\n";
}

// 진출
void SceneBase::OnExit()
{
    //std::cout << "[SceneBase] OnExit\n";
}




void SceneBase::Update(float dt)
{
    m_lastDt = dt;

    if (m_objectManager)
    {
        m_objectManager->Update(dt);
        m_objectManager->WidgetUpdate(dt);
    }

    m_audioScene->Update(dt);

}

void SceneBase::SubmitObj()
{
    if (m_objectManager) 
    {
        m_objectManager->ProcessPending();
        m_objectManager->Submit(m_lastDt);
    }

}

void SceneBase::SubmitUI()
{
    if (m_objectManager)
    {
        m_objectManager->ProcessWidgetPending();
        m_objectManager->WidgetSubmit(m_lastDt);
    }
}

bool SceneBase::OnCreateScene()
{
    // 각 씬에서 여기에 씬 생성시 할 행동 구현하면 됨

    return true;
}

void SceneBase::OnDestroyScene()
{
    // 각 씬에서 여기에 씬 파괴시 할 행동 구현하면 됨
}
