#pragma once
#include "IScene.h"
#include "RenderTypes.h"


class IRenderer;

class SecondScene final : public IScene
{
public:
    void OnEnter() override;
    void OnExit() override;
    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

private:
    MeshHandle     m_mesh = 0;
    MaterialHandle m_mat = 0;
    DirectX::XMFLOAT4X4 m_world{};
};
