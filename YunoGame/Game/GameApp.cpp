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
   m_triangle = std::make_unique<Triangle>();
   if (!m_triangle->Create(DirectX::XMFLOAT3(0, 0, -5)))
       return false;

    return true;
}

void GameApp::OnUpdate(float dt)
{
    static float acc = 0.0f;
    static int frameCount = 0;

    acc += dt;
    ++frameCount;

    static float test = 0.0f;
    test += dt;

    if (test >= 4.0f) 
    {
        IRenderer* renderer = YunoEngine::GetRenderer();
        int msaaVal = renderer->GetMSAASamples();
        std::cout <<"before msaa Value = " << msaaVal;
        if(msaaVal == 1)
            renderer->SetMSAASamples(8);
        else
            renderer->SetMSAASamples(1);

        msaaVal = renderer->GetMSAASamples();
        std::cout << ", After msaa Value = " << msaaVal<<std::endl;

        test = 0.0f;
    }

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
        m_quad->Submit(dt);
    }

    if (m_triangle)
    {
        m_triangle->Update(dt);
        m_triangle->Submit(dt);
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
내 메모장임 - 준혁
게임쪽에서 정점버퍼,인덱스 버퍼, 플래그를 넘김 
CreateMesh(const VertexStreams& streams, const uint32_t* indices, uint32_t indexCount) 이 함수 사용
그리고
머테리얼핸들을 받기위해서 머테리얼 관련 데이터들을 넘길거임 (PBR 베이스) 

감마 컬렉션 넣어야 됨

*/