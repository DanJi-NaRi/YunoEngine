#include "pch.h"

#include "YunoEngine.h"
#include "ISceneManager.h"
#include "IInput.h"
#include "IRenderer.h"

#include "MainScene.h"
#include "SecondScene.h"

void SecondScene::OnEnter()
{

    XMFLOAT3 pos = { 0,0,-10 };
    
    XMMATRIX Trans = XMMatrixTranslation( pos.x, pos.y,pos.z );
    

    XMStoreFloat4x4(&m_world, Trans);
}

void SecondScene::OnExit()
{
}

void SecondScene::Update(float dt)
{
    (void)dt;

    // 예시: F1 누르면 MainScene으로 전환
    IInput* input = YunoEngine::GetInput();
    if (!input) return;

    if (input->IsKeyDown(0x70))
    {
        if (auto* sm = YunoEngine::GetSceneManager())
        {
            sm->RequestSetActive(std::make_unique<MainScene>());
        }
    }
}


void SecondScene::Submit(IRenderer* renderer)
{
    if (!renderer) return;

    if (m_mesh == 0)
    {
        // MainScene과 다른 위치/크기 삼각형
        static const VERTEX_Pos verts[3] =
        {
            {  0.0f,   5.0f, 0.0f },
            {  10.0f, -5.0f, 0.0f },
            { -10.0f, -5.0f, 0.0f },
        };

        VertexStreams streams{};
        streams.flags = VSF_Pos;
        streams.pos = verts;
        streams.vtx_count = 3;

        m_mesh = renderer->CreateMesh(streams, nullptr, 0);
    }

    m_mat = 0;
    if (m_mesh == 0) return;

    RenderItem item{};
    item.meshHandle = m_mesh;
    item.materialHandle = m_mat;
    item.mWorld = m_world;

    renderer->Submit(item);
}
