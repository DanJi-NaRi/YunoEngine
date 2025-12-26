#pragma once

#include "IGameApp.h"
#include "RenderTypes.h"


class GameApp : public IGameApp
{
public:
    bool OnInit() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float fixedDt) override;
    void OnShutdown() override;

private:
    MeshHandle m_triangleMesh = 0;
    MaterialHandle m_defaultMaterial = 0;
    DirectX::XMFLOAT4X4 m_world;
    DirectX::XMFLOAT4X4 m_world2;
};
