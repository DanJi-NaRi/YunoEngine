#include "pch.h"
#include "GameApp.h"

#include "IRenderer.h"
#include "YunoEngine.h"

bool GameApp::OnInit()
{
    std::cout << "[GameApp] OnInit\n";

    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
    {
        std::cout << "[GameApp] Renderer not available.\n";
        return false;
    }

    const std::array<VERTEX_Pos, 3> vertices = {
        VERTEX_Pos{DirectX::XMFLOAT3{0.0f, 0.5f, 0.0f}},
        VERTEX_Pos{DirectX::XMFLOAT3{0.5f, -0.5f, 0.0f}},
        VERTEX_Pos{DirectX::XMFLOAT3{-0.5f, -0.5f, 0.0f}},
    };

    VertexStreams streams{};
    streams.flags = VSF_Pos;
    streams.vtx_count = static_cast<uint32_t>(vertices.size());
    streams.pos = vertices.data();

    m_triangleMesh = renderer->CreateMesh(streams, nullptr, 0);
    m_defaultMaterial = renderer->CreateMaterial_Default();
    DirectX::XMStoreFloat4x4(&m_world, DirectX::XMMatrixIdentity());


    XMMATRIX Translate = DirectX::XMMatrixTranslation(0.3f, -0.5f, -0.5f);

    DirectX::XMStoreFloat4x4(&m_world2, Translate);
    if (m_triangleMesh == 0 || m_defaultMaterial == 0)
    {
        std::cout << "[GameApp] Failed to create triangle resources.\n";
        return false;
    }

    return true;
}

void GameApp::OnUpdate(float dt)
{
    static float acc = 0.0f;
    static int frameCount = 0;

    acc += dt;
    ++frameCount;

    if (acc >= 1.0f)
    {
        std::cout << "[GameApp] dt = " << dt << "\n";
        const float fps = static_cast<float>(frameCount) / acc;
        std::cout << "[GameApp] FPS = " << fps << "\n";
        acc = 0.0f;
        frameCount = 0;
    }

    if (IRenderer* renderer = YunoEngine::GetRenderer())
    {
        RenderItem item{};
        item.mesh = m_triangleMesh;
        item.material = m_defaultMaterial;
        item.world = m_world;
    
        renderer->Submit(item);
    }

    if (IRenderer* renderer = YunoEngine::GetRenderer())
    {
        RenderItem item{};
        item.mesh = m_triangleMesh;
        item.material = m_defaultMaterial;
        item.world = m_world2;

        renderer->Submit(item);
    }
}

void GameApp::OnFixedUpdate(float fixedDt)
{
    static int step = 0;
    ++step;

    if (step % 60 == 0)
    {
        std::cout << "[GameApp] FixedUpdate dt = " << fixedDt << "\n";
    }
}

void GameApp::OnShutdown()
{
    std::cout << "[GameApp] OnShutdown\n";
}

/*
게임쪽에서 정점버퍼,인덱스 버퍼, 플래그를 넘김 
CreateMesh(const VertexStreams& streams, const uint32_t* indices, uint32_t indexCount) 이 함수 사용
그리고
머테리얼핸들을 받기위해서 머테리얼 관련 데이터들을 넘길거임 (PBR 베이스) 


// 샘플러 세팅 규약
슬롯 0 = Custom
슬롯 1 = Wrap, Anisotropic
슬롯 2 = Clamp, Anisotropic
슬롯 3 = Wrap, Anisotropic


*/