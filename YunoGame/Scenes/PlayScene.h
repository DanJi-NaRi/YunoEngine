#pragma once

#include "SceneBase.h"
#include "Game_InputContext.h"
#include "IGridSystem.h"
#include "SceneState.h"

//class Game_InputContext;
class Triangle;

class PlayScene final : public SceneBase
{
public:

    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "PlayScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    void TestInput();
    void HandleCardSelect(int key, int unit, int index);
    void AddCardSelect();
    void HandleDirectionInput();
    void EndTurn();

    void RegisterEffect();

    void MoveCamera(float dt);

private:
    std::unique_ptr<IGridSystem> m_playGrid;
    Game_InputContext m_gameCtx;

    uint32_t m_pendingCardRuntimeID = 0; //방향 선택 확인

    int m_selectedUnitSlot = 0; // 어떤 유닛인지 선택

    CurrentSceneState m_CurSceneState;
    CurrentSceneState m_PrevSceneState;

    float m_CamMoveTime = 1.0f;
    float m_curMoveTime = 0.0f;

    XMFLOAT3 m_CurCamPos;
    XMFLOAT3 m_NextCamPos;

    bool isCamMove = false;
};
