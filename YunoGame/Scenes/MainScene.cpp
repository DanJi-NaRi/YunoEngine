#include "pch.h"

#include "YunoEngine.h"
#include "ISceneManager.h"
#include "IInput.h"
#include "IRenderer.h"


#include "SecondScene.h"
#include "MainScene.h"




void MainScene::OnEnter()
{
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

void MainScene::OnExit()
{
}

void MainScene::Update(float dt)
{
    (void)dt;

    // 예시: F2 누르면 SecondScene으로 전환
    IInput* input = YunoEngine::GetInput();
    if (!input) return;

    if (input->IsKeyDown(0x71))
    {
        if (auto* sm = YunoEngine::GetSceneManager())
        {
            sm->RequestSetActive(std::make_unique<SecondScene>());
        }
    }
}

void MainScene::Submit(IRenderer* renderer)
{
    if (!renderer) return;

    // 첫 프레임에 리소스 생성하는 lazy-init 방식
    if (m_mesh == 0)
    {
        // 삼각형 (VERTEX_Pos: float x,y,z)
        static const VERTEX_Pos verts[3] =
        {
            {   0.0f,  5.0f, 0.0f },
            {  10.0f, -5.0f, 0.0f },
            { -10.0f, -5.0f, 0.0f },
        };

        VertexStreams streams{};
        streams.flags = VSF_Pos;
        streams.pos = verts;
        streams.vtx_count = 3;

        m_mesh = renderer->CreateMesh(streams, nullptr, 0);
    }

    // 기본 머테리얼 사용(0이면 YunoRenderer::Submit에서 default로 보정)
    m_mat = 0;

    if (m_mesh == 0) return;

    RenderItem item{};
    item.meshHandle = m_mesh;
    item.materialHandle = m_mat;
    item.mWorld = m_world;

    renderer->Submit(item);
}