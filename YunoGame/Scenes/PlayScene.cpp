#include "pch.h"
#include "PlayScene.h"

#include "Triangle.h"
#include "YunoEngine.h"
#include "IInput.h"
#include "ISceneManager.h"

#include "TitleScene.h"


bool PlayScene::OnCreate()
{
    std::cout << "[PlayScene] OnCreate\n";

    // 플레이어(삼각형)
    m_player = new Triangle();
    if (!m_player->Create({ 0.0f, 0.0f, 0.0f }))
    {
        std::cout << "[PlayScene] Triangle Create failed\n";
        delete m_player;
        m_player = nullptr;
        return false;
    }
    return true;
}

void PlayScene::OnDestroy()
{
    std::cout << "[PlayScene] OnDestroy\n";
    delete m_player;
    m_player = nullptr;
}

void PlayScene::Update(float dt)
{
    m_lastDt = dt;


    IInput* input = YunoEngine::GetInput();
    ISceneManager* sm = YunoEngine::GetSceneManager();
    if (input && sm)
    {
        if ( input->IsKeyPressed('1'))
        {
            std::cout << "[PlayScene] Back -> ReplaceRoot(TitleScene)\n";
            SceneTransitionOptions opt{};
            opt.immediate = true;
            sm->RequestReplaceRoot(std::make_unique<TitleScene>(), opt);
        }
    }

    if (m_player)
        m_player->Update(dt);
}

void PlayScene::Submit(IRenderer* renderer)
{
    (void)renderer;
    if (m_player)
        m_player->Submit(m_lastDt);
}
