#include "pch.h"
#include "TitleScene.h"

#include "Quad.h"
#include "Building.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "ISceneManager.h"

#include "PlayScene.h"
#include "ObjectManager.h"


bool TitleScene::OnCreate()
{
    //std::cout << "[TitleScene] OnCreate\n";

    // 타이틀 바닥(플레인)
    m_plane = new Quad();
    if (!m_plane->Create({ 0.0f, 0.0f, 0.0f }))
    {
        std::cout << "[TitleScene] Quad Create failed\n";
        delete m_plane;
        m_plane = nullptr;
        return false;
    }

    m_ObjManager = new ObjectManager;

    m_ObjManager->Init();

    m_building = m_ObjManager->CreateObjectFromFile<Building>("Buliding", XMFLOAT3(0, 0, 0), "../Assets/fbx/Building/building.fbx");

    return true;
}

void TitleScene::OnDestroy()
{
    //std::cout << "[TitleScene] OnDestroy\n";
    delete m_plane;
    m_plane = nullptr;
}

void TitleScene::Update(float dt)
{
    m_lastDt = dt;

    IInput* input = YunoEngine::GetInput();
    ISceneManager* sm = YunoEngine::GetSceneManager();
    if (input && sm)
    {
        if ( input->IsKeyPressed('2'))
        {
            SceneTransitionOptions opt{};
            opt.immediate = true;
            sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);
        }
        if (input->IsKeyPressed('3'))
        {
            SceneTransitionOptions opt{};
            opt.immediate = true;
            sm->RequestPop();
        }
    }

    if (m_plane)
        m_plane->Update(dt);

    m_ObjManager->Update(dt);

}

void TitleScene::Submit(IRenderer* renderer)
{
<<<<<<< HEAD
    //if (m_plane)
    //    m_plane->Submit(m_lastDt);

    // 프레임 데이터 서브밋 (라이트)
    //  for (objManager -Submit)
    //
    // UI view proj
=======
    (void)renderer; // Unit::Submit�� ���ο��� renderer�� ���� �ʿ� ����
<<<<<<< HEAD
<<<<<<< HEAD
    /*if (m_plane)
        m_plane->Submit(m_lastDt);*/
=======
    if (m_plane)
        m_plane->Submit(m_lastDt);
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
    /*if (m_plane)
        m_plane->Submit(m_lastDt);*/
>>>>>>> 2e8bd5d (좌표계 수정)

    m_ObjManager->Submit(m_lastDt);

    m_ObjManager->ProcessPending();
<<<<<<< HEAD
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
}
