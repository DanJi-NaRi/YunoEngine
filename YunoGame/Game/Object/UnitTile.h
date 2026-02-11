#pragma once
#include "AnimationUnit.h"
#include "YunoTransform.h"
#include "PieceHelper.h"
#include "BattlePackets.h"

class EffectManager;

class UnitTile : public AnimationUnit
{
public:
    explicit UnitTile();
    virtual ~UnitTile();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

private:
    void UpdateFlash(float dt);
public:
    void SetFlashColor(Float4 color, int count, float blinkTime);
    void PlayWarning(ObstacleType obstacleType, Float4 color, int count, float blinkTime);
    void PlayTrigger(ObstacleType obstacleType, Float4 color, int count, float blinkTime);
private:
    Float4 GetLerpColor(float dt);
    float Graph(float x);

private:
    // 장애물 애니메이션
    bool isTriggering = false;
    bool isWarning = false;
    ObstacleType m_state = ObstacleType::None;

    // 반짝
    Float4 m_flashColor{ 1, 1, 1, 1 };// 알람 색깔
    float m_flashTime = 0;            // 현재 누적 시간
    float m_blinkTime = 0.5f;         // 한 번 반짝이는 시간
    int m_count = 0;                  // 몇번 번쩍일 건지
    bool isFlashing = false;
    bool isCollapsed = false;

    bool isDone = false;
};
