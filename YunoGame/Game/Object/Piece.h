#pragma once
#include "Unit.h"
#include "Widget.h"
#include "PieceHelper.h"


template<typename T>
class Piece : public T
{
public:
    explicit Piece();
    template<typename... Args>
    Piece(Args&& ...args) : T(std::forward<Args>(args)...) {}
    
    virtual ~Piece();

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

extern template class Piece<Unit>;
extern template class Piece<Widget>;

VERTEX_Pos g_tMesh[];
VERTEX_UV g_tUV[];
INDEX g_tIndex[];

template<>
bool Piece<Widget>::CreateMesh();

template<>
bool Piece<Widget>::CreateMaterial();

template<>
bool Piece<Widget>::UpdateMatrix();
