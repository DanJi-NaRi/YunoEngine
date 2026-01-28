#include "pch.h"

#include "YunoSceneManager.h"

#include "IScene.h"
#include "SceneBase.h"
#include "SerializeScene.h"
#include "IRenderer.h"

#include "ImGuiManager.h"
#include "UImgui.h"


SceneEntry::~SceneEntry() = default;
PendingOp::~PendingOp() = default;


YunoSceneManager::~YunoSceneManager()
{
    m_pendingNow.clear();
    m_pendingNext.clear();

    ShutdownStack();
}

inline std::string WStringToUtf8(const std::wstring& w)
{
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), size, nullptr, nullptr);
    return s;
}

inline std::wstring Utf8ToWString(const std::string& s)
{
    if (s.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), size);
    return w;
}

#ifdef _DEBUG
void YunoSceneManager::RegisterDrawSceneUI()
{
    ImGuiManager::RegisterDraw(
        [this]() {
            UI::BeginPanel("SceneHierarchy");

            for (size_t i = 0; i < m_views.size(); i++)
            {
                auto& pView = m_views[i];

                bool selected = m_selectView != nullptr ? (m_selectView->stackIndex == pView->stackIndex) : false;
                if (UI::Selectable(pView->name.c_str(), selected))
                {
                    m_selectView = pView.get();
                }
            }

            UI::EndPanel();
        }
    );

    ImGuiManager::RegisterDraw(
        [this]() {
            using namespace nlohmann;

            UI::SetNextUIPos(1, 50);
            UI::SetNextUISize(120, 60);
            UI::BeginPanel("Save/Load");

            if (UI::Button("Save"))
            {
                if (m_selectView)
                {
                    std::string scenename = m_stack[m_selectView->stackIndex].scene->GetDebugName();
                    std::wstring path = L"../Assets/Scenes/" + Utf8ToWString(scenename) + L".json";
                    SaveSceneToFile(m_stack[m_selectView->stackIndex].scene->BuildSceneDesc(), path);
                }
            }

            UI::SameLine(60);

            if (UI::Button("Load"))
            {
                
            }

            UI::EndPanel();
        }
    );

    ImGuiManager::RegisterDraw(
        [this]() {
            UI::SetNextUISize(200, 500);
            UI::BeginPanel("ObjectHierarchy");

            if (m_selectView)
            {
                dynamic_cast<SceneBase*>(m_stack[m_selectView->stackIndex].scene.get())->DrawObjectList();
            }
            UI::EndPanel();
        }
    );


    ImGuiManager::RegisterDraw(
        [this]() {
            UI::SetNextUISize(400, 500);
            UI::BeginPanel("Inspector");

            if (m_selectView)
            {
                dynamic_cast<SceneBase*>(m_stack[m_selectView->stackIndex].scene.get())->DrawInspector();
            }

            UI::EndPanel();
        }
    );
}
#endif


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

void YunoSceneManager::CreateView(const SceneEntry& e, UINT idx)
{
    if (!e.scene) return;

    auto v = std::make_unique<SceneView>();
    v->name = std::string(e.scene->GetDebugName()) + std::to_string(m_views.size());
    v->stackIndex = idx;
    v->state = &e.state;

    m_views.push_back(std::move(v));
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
            m_views.clear();
            m_selectView = nullptr;

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
            CreateView(m_stack[m_stack.size() - 1], m_stack.size() - 1);

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
            CreateView(m_stack[m_stack.size() - 1], m_stack.size() - 1);
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
                m_views.pop_back();
                m_selectView = nullptr;
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

void YunoSceneManager::SubmitAndRender(IRenderer* renderer)
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

#if defined(_DEBUG)
    renderer->DrawDebug();
#endif

    for (int i = start; i < static_cast<int>(m_stack.size()); ++i)
    {
        SceneEntry& e = m_stack[i];

        if (!EnsureCreated(e)) continue;
        e.scene->SubmitObj();

        renderer->DrawShadowMap();
        renderer->Flush();
    }

    renderer->PostProcess();

    for (int i = start; i < static_cast<int>(m_stack.size()); ++i)
    {
        SceneEntry& e = m_stack[i];

        if (!EnsureCreated(e)) continue;
        e.scene->SubmitUI();

        renderer->Flush();

    }
}
