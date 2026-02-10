#include "pch.h"
#include "ResultScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"

bool ResultScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);


    return true;
}


void ResultScene::OnDestroyScene()
{
}

void ResultScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
    winner = GameManager::Get().GetWinnerPID();
}

void ResultScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void ResultScene::Update(float dt)
{
    SceneBase::Update(dt);

}

void ResultScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void ResultScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
