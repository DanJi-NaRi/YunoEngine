#pragma once
#include "AnimationUnit.h"
#include "PieceHelper.h"

class UnitPiece : public AnimationUnit
{
public:
    explicit UnitPiece();

    virtual ~UnitPiece();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

    bool UpdateMatrix();

    void UpdateFlash(float dt);
public:
    void InsertQ(PGridCmd targetPos);
    void SetWho(GamePiece type);
    void SetDir(Direction dir, bool isAnim = true);
    void SetFlashColor(Float4 color, int count, float blinkTime);
    void SetDead();

    void SetTmpColor(Float4 color);
private:
    void SetTarget(XMFLOAT3 targetPos, float speed);
    void SendDone();
    void ClearQ();

    Float4 GetLerpColor(float dt);
    float Graph(float x);

private:
    bool m_AnimDone = false;

    // 반짝
    Float4 m_vtmpColor{ 1, 1, 1, 1 };
    Float4 m_flashColor{ 1, 1, 1, 1 };
    float m_flashTime = 0;
    float m_blinkTime = 0.5f;
    int m_count = 0;
    bool isFlashing = false;

    // 이동
    XMVECTOR m_Target{};
    XMVECTOR m_Start{};
    float m_Dist = 0;
    float m_speed = 10;
    float m_fixSpeed = 1;
    float m_AnimTime = 0.f;
    bool isMoving = false;

    // 회전
    float m_rotTime = 0.f;
    float m_rotSpeed = 2.f;
    float m_yaw = atan2(0, 1);
    float m_startYaw = 0;
    float m_targetYaw = 0;
    bool isRotating = false;

    // 죽음 체크
    bool isDead = false;
    float m_deadTime = 0.f;
    float m_deathDelay = 1.f;

    GamePiece m_who = GamePiece::None;

    // 큐
    std::queue<PGridCmd> m_Q;
};
