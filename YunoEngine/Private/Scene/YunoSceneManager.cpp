#include "pch.h"

#include "YunoSceneManager.h"

#include "IScene.h"
#include "IRenderer.h"

// 테스트
#include "YunoEngine.h"

SceneEntry::~SceneEntry() = default;
PendingOp::~PendingOp() = default;

YunoSceneManager::~YunoSceneManager()
{
    m_pendingNow.clear();
    m_pendingNext.clear();

    ShutdownStack();
};


static inline void EnqueueOp(std::vector<PendingOp>& now, std::vector<PendingOp>& next, PendingOp&& op)
{
    if (op.opt.immediate) now.push_back(std::move(op));
    else                 next.push_back(std::move(op));
}

void YunoSceneManager::RequestReplaceRoot(std::unique_ptr<IScene> scene, SceneTransitionOptions opt)
{
    if (!scene) return;

    PendingOp op{};
    op.type = PendingOpType::ReplaceRoot;
    op.scene = std::move(scene);
    op.opt = opt;

    EnqueueOp(m_pendingNow, m_pendingNext, std::move(op));
}

void YunoSceneManager::RequestPush(std::unique_ptr<IScene> scene, ScenePolicy policy, SceneTransitionOptions opt)
{
    if (!scene) return;

    PendingOp op{};
    op.type = PendingOpType::Push;
    op.scene = std::move(scene);
    op.policy = policy;
    op.opt = opt;

    EnqueueOp(m_pendingNow, m_pendingNext, std::move(op));
}

void YunoSceneManager::RequestPop(SceneTransitionOptions opt)
{
    PendingOp op{};
    op.type = PendingOpType::Pop;
    op.opt = opt;

    EnqueueOp(m_pendingNow, m_pendingNext, std::move(op));
}

IScene* YunoSceneManager::GetActiveScene() const
{
    if (m_stack.empty()) return nullptr;
    return m_stack.back().scene.get();
}

bool YunoSceneManager::EnsureCreated(SceneEntry& e)
{
    if (!e.scene) return false;

    if (e.state == SceneState::Uninitialized)   // 씬이 만들어지지 않았다면 Create호출
    {
        const bool ok = e.scene->OnCreate();
        if (!ok)
        {

            e.scene->OnDestroy();
            e.scene.reset();
            return false;
        }
        e.state = SceneState::Loaded;
    }
    return true;
}

void YunoSceneManager::DumpStack_Console(const char* reason) const
{
    std::cout << "\n[SceneStack] "
        << (reason ? reason : "")
        << " | size=" << m_stack.size() << "\n";

    for (size_t i = 0; i < m_stack.size(); ++i)
    {
        const SceneEntry& e = m_stack[i];

        std::cout
            << "  [" << i << "] "
            << (e.scene ? e.scene->GetDebugName() : "(null)")
            << " | state=" << static_cast<int>(e.state)
            << "\n\n";
    }
}

void YunoSceneManager::ShutdownStack()
{
    while (!m_stack.empty())
    {
        SceneEntry& top = m_stack.back();

        if (top.scene)
        {
            if (top.state == SceneState::Active || top.state == SceneState::Paused)
                top.scene->OnExit();

            top.scene->OnDestroy();
        }

        m_stack.pop_back();
    }
}

void YunoSceneManager::PromoteNextPending()
{
    if (m_pendingNext.empty())
        return;


    for (auto& op : m_pendingNext)
        m_pendingNow.push_back(std::move(op));

    m_pendingNext.clear();
}

void YunoSceneManager::ApplyPending(std::vector<PendingOp>& ops)
{
    if (ops.empty())
        return;

    for (auto& op : ops)
    {
        if (op.type == PendingOpType::ReplaceRoot)
        {

            while (!m_stack.empty())
            {
                SceneEntry& top = m_stack.back();
                if (top.scene)
                {
                    if (top.state == SceneState::Active || top.state == SceneState::Paused)
                        top.scene->OnExit();

                    top.scene->OnDestroy();
                }
                m_stack.pop_back();
            }

            // 새 씬 1개로 교체
            SceneEntry e{};
            e.scene = std::move(op.scene);
            e.policy = op.policy;

            if (!EnsureCreated(e))
            {
                continue;
            }

            e.scene->OnEnter();
            e.state = SceneState::Active;
            m_stack.push_back(std::move(e));
            DumpStack_Console("After ReplaceRoot");
        }
        else if (op.type == PendingOpType::Push)
        {
            // 기존 top Pause
            if (!m_stack.empty())
            {
                SceneEntry& prevTop = m_stack.back();
                EnsureCreated(prevTop);
                if (prevTop.scene && prevTop.state == SceneState::Active)
                {
                    prevTop.scene->OnPause();
                    prevTop.state = SceneState::Paused;
                }
            }

            SceneEntry e{};
            e.scene = std::move(op.scene);
            e.policy = op.policy;

            if (!EnsureCreated(e))
            {
                continue;
            }

            e.scene->OnEnter();
            e.state = SceneState::Active;
            m_stack.push_back(std::move(e));
            DumpStack_Console("After Push");
        }
        else if (op.type == PendingOpType::Pop)
        {
            if (m_stack.empty())
                continue;

            // top 종료/파괴
            {
                SceneEntry& top = m_stack.back();
                EnsureCreated(top);
                if (top.scene)
                {
                    if (top.state == SceneState::Active)
                        top.scene->OnExit();

                    top.scene->OnDestroy();
                }
                m_stack.pop_back();
            }

            // 새 top Resume
            if (!m_stack.empty())
            {
                SceneEntry& newTop = m_stack.back();
                EnsureCreated(newTop);
                if (newTop.scene && newTop.state == SceneState::Paused)
                {
                    newTop.scene->OnResume();
                    newTop.state = SceneState::Active;
                }
            }
            DumpStack_Console("After Pop");
        }
    }

    ops.clear();
}

void YunoSceneManager::Update(float dt)
{

    PromoteNextPending();


    ApplyPending(m_pendingNow);

    if (m_stack.empty())
        return;


    for (int i = static_cast<int>(m_stack.size()) - 1; i >= 0; --i)
    {
        SceneEntry& e = m_stack[i];
        if (!EnsureCreated(e)) continue;
        e.scene->Update(dt);

        if (e.policy.blockUpdateBelow)
            break;
    }
}

void YunoSceneManager::Submit()
{
    if (m_stack.empty()) return;

    int start = 0;
    for (int i = static_cast<int>(m_stack.size()) - 1; i >= 0; --i)
    {
        if (m_stack[i].policy.blockRenderBelow)
        {
            start = i;
            break;
        }
    }

    for (int i = start; i < static_cast<int>(m_stack.size()); ++i)
    {
        SceneEntry& e = m_stack[i];

        if (!EnsureCreated(e)) continue;
        e.scene->Submit();

        //IRenderer* s_renderer = YunoEngine::GetRenderer();


        //std::cout << "씬 렌더" << std::endl;
    }
}
