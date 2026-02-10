#pragma once
#include "AnimTest.h"
#include "PieceHelper.h"

class EffectManager;
class UnitPiece : public AnimTest
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

    bool CheckDead(float dt);
    void CheckMyQ();

    bool UpdateMatrix();
    void UpdateRot(float dt);
    void UpdateMove(float dt);
    void UpdateFlash(float dt);
    void UpdateAttack(float dt);
    void UpdateRollingOrBack(float dt);
    void UpdateHit(float dt);
    void UpdateDissolve(float dt);

public:
    void InsertQ(PGridCmd targetPos);
    void SetWho(GamePiece type, Team team, uint8_t weaponID, uint8_t subID = 0);
    void SetMoveRotOffset(float moveOffset, float rotOffset);
    void SetIgnoreRot(bool ignore);
    float GetMoveOffset();
    void SetDir(Direction dir, bool isAnim = true, float second = 1.f);
    void SetFlashColor(Float4 color, int count, float blinkTime);
    void AppearDissolve(float dissolveTime);
    void DisappearDissolve(float dissolveTime);

    void SetTmpColor(Float4 color);

private:
    void PlayMove(XMFLOAT3 targetPos, float speed);
    void PlayRollBack(float seconds = 0);
    void PlayRoll(float seconds);
    void PlayAttack();
    void PlayHit();
    void PlayDead(float disappearDisolveDuration);

    Float4 GetLerpColor(float dt);
    float GetLerp(float dt, float start, float end);
    float QuadraticGraph(float x);
    float linearGraph(float x);

    void ClearQ();

private:
    bool m_AnimDone = false;

    // 그래프
    float m_linearSlope = 0.f;

    // 디졸브
    bool isDissolving = false;
    float m_dissolveTime = 0.f;
    float m_dissolveDuration = 0.f;
    float m_startDissolveAmount = 0.f;

    // 공격
    bool isAttacking = false;
    bool isRollingBack = false;
    bool isRolling = false;
    float m_rollTime = 0.f;
    float m_rollorbackSpeed = 4.f;

    // 피격
    bool isHitting = false;

    // 반짝
    Float4 m_vtmpColor{ 1, 1, 1, 1 };
    Float4 m_flashColor{ 1, 1, 1, 1 };
    float m_flashTime = 0;
    float m_blinkTime = 0.5f;
    int m_count = 0;
    bool isFlashing = false;

    // 이동
    float m_moveOffset = -0.5f;
    float m_curMoveOffset = 0.f;
    XMVECTOR m_Target{};
    XMVECTOR m_Start{};
    float m_Dist = 0;
    float m_speed = 10;
    float m_fixSpeed = 1;
    float m_moveTime = 0.f;
    bool isMoving = false;

    // 회전
    float m_rotOffset = XMConvertToRadians(30.f);
    float m_curRotOffset = 0.f;
    Direction m_dir = Direction::None;
    float m_rotTime = 0.f;
    float m_rotSpeed = 2.f;
    float m_yaw = atan2(0, 1);
    float m_startYaw = 0;
    float m_targetYaw = 0;
    bool isRotating = false;
    bool ignoreRot = false;

    // 죽음 체크
    bool isDead = false;
    bool isDeadQueued = false;

    // 본인 기물 정보
    GamePiece m_who = GamePiece::None;
    Team m_team = Team::Undefined;
    uint8_t m_weaponID = 0;
    uint8_t m_subID = 0;                // 차크람일 경우, 서브 아이디
    uint8_t charkramID = 2;             // 챠크람 아이디

    // 큐
    std::queue<PGridCmd> m_Q;

    //EffectManager* m_effectManager = nullptr;
};
