#pragma once
#include "Widget.h"
#include "PieceHelper.h"

class PGridCmd;

class WidgetPiece : public Widget
{
public:
    explicit WidgetPiece(UIFactory& uiFactory);//m_name = L"Piece";

    virtual ~WidgetPiece();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

    bool UpdateMatrix();

public:
    void InsertQ(PGridCmd targetPos);
    void SetWho(GamePiece type);
    void SetDir(Direction dir, bool isAnim = true);
    void SetDead();

private:
    void SetTarget(XMFLOAT3 targetPos, float speed);
    void SendDone();
    void ClearQ();

private:
    bool m_AnimDone = false;

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
