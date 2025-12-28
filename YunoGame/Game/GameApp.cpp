#include "pch.h"




#include "RenderTypes.h"
#include "IRenderer.h"
#include "YunoEngine.h"

#include "GameApp.h"



GameApp::~GameApp() = default;

bool GameApp::OnInit()
{
    std::cout << "[GameApp] OnInit\n";

    IRenderer* renderer = YunoEngine::GetRenderer();
    if (!renderer)
    {
        std::cout << "[GameApp] Renderer not available.\n";
        return false;
    } // 렌더러 체크

   m_quad = std::make_unique<Quad>();
   if (!m_quad->Create(DirectX::XMFLOAT3(0, 0, 0)))
       return false;

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

    if (m_quad)
    {
        m_quad->Update(dt);
        m_quad->Submit();  
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