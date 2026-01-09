#pragma once
#include "IScene.h"
#include "RenderTypes.h"


class IRenderer;
class IInput;


class MainScene final : public IScene
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

private:
    // 최소 샘플: 메쉬/머테리얼 핸들 하나씩
    MeshHandle     m_mesh = 0;
    MaterialHandle m_mat = 0;

    DirectX::XMFLOAT4X4 m_world{};
};
