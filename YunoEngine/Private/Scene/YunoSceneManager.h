#pragma once
#include "ISceneManager.h"


class IScene;



struct SceneEntry
{
    SceneEntry() = default;
    ~SceneEntry();

    SceneEntry(SceneEntry&&) noexcept = default;
    SceneEntry& operator=(SceneEntry&&) noexcept = default;

    SceneEntry(const SceneEntry&) = delete;
    SceneEntry& operator=(const SceneEntry&) = delete;

    std::unique_ptr<IScene> scene;
    SceneState state = SceneState::Uninitialized;
    ScenePolicy policy{};
};

enum class PendingOpType : uint8_t { ReplaceRoot, Push, Pop };

struct PendingOp
{
    PendingOp() = default;
    ~PendingOp();

    PendingOp(PendingOp&&) noexcept = default;
    PendingOp& operator=(PendingOp&&) noexcept = default;

    PendingOp(const PendingOp&) = delete;
    PendingOp& operator=(const PendingOp&) = delete;

    PendingOpType type{};
    std::unique_ptr<IScene> scene; 
    ScenePolicy policy{};
    SceneTransitionOptions opt{};
};

class YunoSceneManager final : public ISceneManager
{
public:
    YunoSceneManager() = default;
    ~YunoSceneManager() override;

    void RequestReplaceRoot(std::unique_ptr<IScene> scene, SceneTransitionOptions opt = {}) override;
    void RequestPush(std::unique_ptr<IScene> scene, ScenePolicy policy = {}, SceneTransitionOptions opt = {}) override;
    void RequestPop(SceneTransitionOptions opt = {}) override;

    void Update(float dt) override;
    void Submit() override;

    IScene* GetActiveScene() const override;
    uint32_t GetStackSize() const override { return static_cast<uint32_t>(m_stack.size()); }

private:
    void ShutdownStack();                               // 종료시 모든 씬 정리
    void PromoteNextPending();
    void ApplyPending(std::vector<PendingOp>& ops);     // 씬 변경 요청 처리
    bool EnsureCreated(SceneEntry& e);                  // 씬 유효성 검증 함수

private: // 디버그용
    void DumpStack_Console(const char* reason) const;

private:
    std::vector<SceneEntry> m_stack;

    std::vector<PendingOp> m_pendingNow;   // immediate=true
    std::vector<PendingOp> m_pendingNext;  // immediate=false (다음 프레임 적용)
};
