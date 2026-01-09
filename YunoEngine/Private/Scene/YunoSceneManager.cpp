#include "pch.h"
#include "YunoSceneManager.h"

YunoSceneManager::~YunoSceneManager()
{
    if (m_active)
    {
        m_active->OnExit();
        m_active.reset();
    }

    m_pending.reset();
}

void YunoSceneManager::RequestSetActive(std::unique_ptr<IScene> next)
{
    if (!next)  
        return;

    m_pending = std::move(next);
}

void YunoSceneManager::ApplyPendingScene()
{
    if (!m_pending)
        return;

    // 기존 씬 종료
    if (m_active)
        m_active->OnExit();

    // 교체
    m_active = std::move(m_pending);

    // 새 씬 시작
    if (m_active)
        m_active->OnEnter();
}

void YunoSceneManager::Update(float dt)
{
    ApplyPendingScene();

    if (m_active)
        m_active->Update(dt);
}

void YunoSceneManager::Submit(IRenderer* renderer)
{

    //ApplyPendingScene();

    if (m_active)
        m_active->Submit(renderer);
}
