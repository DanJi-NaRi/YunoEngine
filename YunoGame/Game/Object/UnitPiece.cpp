#include "pch.h"

#include "PlayQueue.h"
#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"
#include "EffectManager.h"
#include "UnitPiece.h"

#include "AudioQueue.h"

UnitPiece::UnitPiece()
{
    m_name = L"Piece";
}

UnitPiece::~UnitPiece()
{
}

bool UnitPiece::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);

    // 오브젝트 처음에는 안보이게 설정
    m_dissolveAmount = 1.f;
    for (auto& mesh : m_Meshs)
    {
        mesh->SetDissolveAmount(m_dissolveAmount);
    }

    Backup();

    return true;
}


bool UnitPiece::Update(float dTime)
{
    // 디졸브 후 죽은 상태인지 먼저 확인
    UpdateDissolve(dTime);
    AnimTest::UpdateDissolve(dTime);
    if (CheckDead(dTime))
    {
        AnimationUpdate(dTime);
        return true;
    }

    UpdateOffset();
    UpdateRot(dTime);
    UpdateMove(dTime);
    UpdateFlash(dTime);
    UpdateRollingOrBack(dTime);
    UpdateAttack(dTime);
    UpdateHit(dTime);

    CheckMyQ();

    UpdateMatrix();
    AnimationUpdate(dTime);

    return true;
}


bool UnitPiece::Submit(float dTime)
{
    AnimationUnit::Submit(dTime);
    return true;
}

bool UnitPiece::CheckDead(float dt)
{
    if (!HasAnimState(PieceAnim::Dead)) return false;
    if (HasAnimState(PieceAnim::DeadQueued)) return true;

    // 다 사라지고 난 뒤에만 시스템에 삭제를 요청한다.
    if (m_dissolveAmount == 1)
    {
        PlayGridQ::Insert(PlayGridQ::Cmd_S(CommandType::Dead, m_who));
        AddAnimState(PieceAnim::DeadQueued);
    }

    return true;
}

void UnitPiece::CheckMyQ()  
{  
   if (m_state == PieceAnim::Dead) return;

   // 애니메이션이 끝나면 하나씩 빼가게 하기  
   while (!m_Q.empty() && m_state == PieceAnim::Idle)
   {  
       auto tp = m_Q.front();  
       m_Q.pop();  
       switch (tp.cmdType)  
       {  
       case CommandType::Rotate:  
       {  
           SetDir(tp.rot_p.dir, true, 0.5f);
           for (auto* sub : m_linkedSubPieces)
           {
               if (sub == nullptr) continue;
               sub->SetDir(tp.rot_p.dir, false);
           }
           break;  
       }  
       case CommandType::Attack:  
       {  
           PlayRollBack();
           //PlayAttack();  
           break;  
       }  
       case CommandType::Move:  
       {  
           auto [wx, wy, wz, second] = tp.mv_p;  
           SetTarget({ wx, wy, wz }, second);  

           break;  
       }  
       case CommandType::MoveHit:  
       {  
           if (tp.hit_p.amIdead)
               PlayDead(tp.hit_p.disappearDissolveDuration);
           else
               PlayHit();

           // MoveHit는 본체가 1회만 시스템 큐에 전달해야 한다.
           // 서브 피스까지 동일 커맨드를 전달받으면 Hit_S가 중복 enqueue되어
           // 상대 피스 hit가 여러 번 재생되거나 대상이 꼬일 수 있다.
           if (m_subID == 0 && tp.hit_p.whichPiece != GamePiece::None)
               PlayGridQ::Insert(PlayGridQ::Hit_S(tp.hit_p.whichPiece));
           
           for (auto* sub : m_linkedSubPieces)
           {
               if (sub == nullptr) continue;
               sub->InsertQ(PlayGridQ::MoveHit_P(tp.hit_p.whichPiece, tp.hit_p.amIdead, tp.hit_p.disappearDissolveDuration));
           }

           break;
       }  
       case CommandType::Hit:  
       {  
           PlayHit();  
           break;  
       }  
       case CommandType::Dead:  
       {  
           PlayDead(tp.dead_p.disappearDissolveDuration);  
           break;  
       }  
       case CommandType::Disappear:  
       {  
           DisappearDissolve(tp.disappear_p.disappearDissolveDuration);  
           break;  
       }  
       }  
   }  
}


bool UnitPiece::UpdateMatrix()
{

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x,m_vScale.y, m_vScale.z);
    XMMATRIX mRot = (ignoreRot)? XMMatrixIdentity() : XMMatrixRotationRollPitchYaw(0, m_yaw, m_curRotOffset);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z + m_curMoveOffset);

    XMMATRIX mTM;

    if (this->m_Parent)
        mTM = mScale * mRot * mTrans * this->m_Parent->GetWorldMatrix();
    else
        mTM = mScale * mRot * mTrans;

    XMStoreFloat4x4(&this->m_mScale, mScale);
    XMStoreFloat4x4(&this->m_mRot, mRot);
    XMStoreFloat4x4(&this->m_mTrans, mTrans);
    XMStoreFloat4x4(&this->m_mWorld, mTM);

    return true;
}

void UnitPiece::UpdateOffset()
{
    bool isAnySubBusy = false;
    for (auto* sub : m_linkedSubPieces)
    {
        if (sub == nullptr) continue;
        if (sub->m_state == PieceAnim::Attack)
        {
            isAnySubBusy = true;
            break;
        }
    }
    m_curRotOffset = (m_state == PieceAnim::Attack || isAnySubBusy) ? 0.f : (m_dir == Direction::Right) ? -m_rotOffset : m_rotOffset;
    m_curMoveOffset = (m_state == PieceAnim::Attack || isAnySubBusy) ? 0.f : m_moveOffset;
}

void UnitPiece::UpdateRot(float dt)
{
    if (m_state != PieceAnim::Rot)   return;
    
    m_rotTime += dt * m_rotSpeed;
    if (m_rotTime >= 1.f)
    {
        m_yaw = m_targetYaw;
        m_rotTime = 0.f;
        m_state = PieceAnim::Idle;
    }
    else
    {
        m_yaw = m_startYaw * (1 - m_rotTime) + m_targetYaw * m_rotTime;
    }
    
}

void UnitPiece::UpdateMove(float dt)
{
    if (m_state != PieceAnim::Move)  return;
    
    m_moveTime += dt / m_Dist * m_speed * m_fixSpeed;

    if (m_moveTime >= 1.f)
    {
        XMStoreFloat3(&this->m_vPos, m_Target);
        
        StopMove();

        for (auto* sub : m_linkedSubPieces)
        {
            if (sub == nullptr) continue;

            sub->StopMove();
        }
    }
    else
    {
        XMVECTOR res = XMVectorLerp(m_Start, m_Target, m_moveTime);
        XMStoreFloat3(&this->m_vPos, res);
    } 
}

void UnitPiece::UpdateFlash(float dt)
{
    if (m_state != PieceAnim::Flash)    return;
    
    // 속도 조절 가능
    m_flashTime += dt;

    // 0에서 시작해 1까지 올라갔다 다시 내려오는 부드러운 곡선
    float btw0_1 = QuadraticGraph(m_flashTime);

    // btw0_1을 기반으로 색상 보간
    Float4 fc = GetLerpColor(btw0_1);
    SetMaskColor({ fc.x, fc.y, fc.z, fc.w });

    if (m_flashTime >= m_blinkTime)
    {
        m_count--;
        m_flashTime = 0.f;
    }
    if (m_count == 0)
    {
        // 종료 시 원래 색 복원
        SetMaskColor({ m_vtmpColor.x, m_vtmpColor.y, m_vtmpColor.z, m_vtmpColor.w });
        m_flashTime = 0.f;
        m_state = PieceAnim::Idle;
    }

}

void UnitPiece::UpdateHit(float dt)
{
    if (m_state != PieceAnim::Hit)  return;

    // animator가 없는 피스(예: 복합 무기 루트)는 히트 상태가 남아
    // 큐가 영구 정지되지 않도록 즉시 정리한다.
    if (m_animator == nullptr)
    {
        m_state = PieceAnim::Idle;
        return;
    }

    if (m_animator)
    {
        bool isPlaying = m_animator->isPlaying();
        if (!isPlaying)
        {
            m_animator->Change("idle");
            m_animator->SetLoop("idle", true);
            m_animator->Play();
            m_state = PieceAnim::Idle;
        }
    }
}

void UnitPiece::UpdateDissolve(float dt)
{
    //if (m_state != PieceAnim::Dissolve) return;
    if (!HasAnimState(PieceAnim::Dissolve)) return;

    m_dissolveTime += dt;

    if (m_dissolveTime >= m_dissolveDuration)
    {
        m_dissolveAmount = m_startDissolveAmount + linearGraph(m_dissolveDuration);
        m_dissolveTime = 0.f;
        m_state = PieceAnim::Idle;
    }
    else
    {
        m_dissolveAmount = m_startDissolveAmount + linearGraph(m_dissolveTime);
    }

    for (auto& mesh : m_Meshs)
    {
        mesh->SetDissolveAmount(m_dissolveAmount);
    }
}

void UnitPiece::UpdateAttack(float dt)
{
    if (m_state != PieceAnim::Attack) return;

    if (m_animator == nullptr)
    {
        // animator가 없는 부모 피스(예: 차크람 루트)는
        // 자식 공격이 끝날 때까지 Attack 상태를 유지해야 오프셋 점프가 발생하지 않는다.
        bool anySubAttacking = false;
        for (auto* sub : m_linkedSubPieces)
        {
            if (sub == nullptr) continue;
            if (sub->HasAnimState(PieceAnim::Attack) || sub->HasAnimState(PieceAnim::RollBack))
            {
                anySubAttacking = true;
                break;
            }
        }

        if (!anySubAttacking)
            m_state = PieceAnim::Idle;

        return;
    }

    bool isPlaying = m_animator->isPlaying();

    if (!isPlaying)
    {
        m_animator->Change("idle");
        m_animator->Play();

        m_state = PieceAnim::Idle;
    }
}

void UnitPiece::UpdateRollingOrBack(float dt)
{
    bool isAnySubBusy = false;
    for (auto* sub : m_linkedSubPieces)
    {
        if (sub == nullptr) continue;
        if (sub->m_state == PieceAnim::Attack)
        {
            isAnySubBusy = true;
            break;
        }
    }

    if (m_state == PieceAnim::RollBack)
    {
        m_rollTime += dt * m_rollorbackSpeed;

        m_curMoveOffset = GetLerp(m_rollTime, m_moveOffset, 0);
        m_curRotOffset = GetLerp(m_rollTime, m_rotOffset, 0);
        
        if (m_rollTime >= 1.f)
        {
            m_curMoveOffset = 0;
            m_curRotOffset = 0;
            m_rollTime = 0;
            
            PlayAttack();
            for (auto* sub : m_linkedSubPieces)
            {
                if (sub == nullptr) continue;
                sub->PlayAttack();
            }
        }
    }
    // 복합 무기(차크람)는 부모에 animator가 없으므로 isAttacking으로는
    // 서브 공격 진행 상태를 알 수 없다. 서브 동작이 끝난 뒤 복귀(roll)하도록 제어한다.
    else if (m_state == PieceAnim::Roll && !isAnySubBusy)
    {
        m_rollTime += dt * m_rollorbackSpeed;

        m_curMoveOffset = GetLerp(m_rollTime, 0, m_moveOffset);
        m_curRotOffset = GetLerp(m_rollTime, 0, m_rotOffset);

        if (m_rollTime >= 1.f)
        {
            m_curMoveOffset = m_moveOffset;
            m_curRotOffset = m_rotOffset;
            m_rollTime = 0;
            m_state = PieceAnim::Idle;
        }
    }
}


void UnitPiece::InsertQ(PGridCmd cmd)
{
    m_Q.push(cmd);
}


void UnitPiece::SetWho(GamePiece type, Team team, uint8_t pID, uint8_t weaponID, uint8_t subID)
{
    m_who = type;
    m_team = team;
    m_pID = pID;
    m_weaponID = weaponID;
    m_subID = subID;
}

void UnitPiece::SetMoveRotOffset(float moveOffset, float rotOffset)
{
    m_moveOffset = moveOffset;
    m_rotOffset = rotOffset;
}

void UnitPiece::SetIgnoreRot(bool ignore)
{
    ignoreRot = ignore;
}

float UnitPiece::GetMoveOffset()
{
    return m_moveOffset;
}


void UnitPiece::SetDir(Direction dir, bool isAnim, float second)
{
    switch (dir)
    {
    case Direction::Same:
        return;
        break;
    case Direction::Right:
        m_dir = Direction::Right;
        m_targetYaw = atan2(-1, 0);
        break;
    case Direction::Left:
        m_dir = Direction::Left;
        m_targetYaw = atan2(1, 0);
        break;
    default:
        return;
        break;
    }
    if (m_targetYaw == m_yaw) return;

    m_startYaw = m_yaw;
    SetAnimState(PieceAnim::Rot, isAnim);
    m_rotSpeed = 1 / second;

    m_yaw = (isAnim) ? m_yaw : m_targetYaw;
}

void UnitPiece::SetFlashColor(Float4 color, int count, float blinkTime)
{
    m_flashColor = color;
    m_count = count;
    m_blinkTime = blinkTime;
    m_state = PieceAnim::Flash;
}

void UnitPiece::AppearDissolve(float dissolveTime)
{
    m_linearSlope = -(1.f / dissolveTime);
    m_dissolveDuration = dissolveTime;
    m_startDissolveAmount = m_dissolveAmount;
    m_state = PieceAnim::Dissolve;
    //AddAnimState(PieceAnim::Dissolve);
}

void UnitPiece::DisappearDissolve(float dissolveTime)
{
    m_linearSlope = 1.f / dissolveTime;
    m_dissolveDuration = dissolveTime;
    m_startDissolveAmount = m_dissolveAmount;

    m_state = PieceAnim::Dissolve | PieceAnim::Dead;
    //AddAnimState(PieceAnim::Dissolve);
    //AddAnimState(PieceAnim::Dead);
    //RemoveAnimState(PieceAnim::DeadQueued);
}

void UnitPiece::PlayAttack()
{
    if (m_animator == nullptr) return;
    bool isChanged = m_animator->Change("attack", (m_weaponID == 2)? 0.1f : 0.5f);   // 챠크람, 대검일 경우에는 블렌딩 시간 0.1f
    if (!isChanged) return;
    m_animator->SetLoop("attack", false);
    m_animator->Play();

    // 이펙트
    if (m_pID == 1)       // Player 1일 경우
    {
        switch (m_weaponID)
        {
        case 1:
        {
            RegisterFrameEvent(1, 36, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::BlasterAttack, { 0.f, -0.05f, -0.05f }, { 1.f, 1.f, 1.f }, { -1, 0, 0 });
                AttachChildBone(eff, 2);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_BlasterAttack));
            break;
        }
        case 2:
        {
            if (m_subID == 1)
            {
                RegisterFrameEvent(1, 10, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttack01_1, { 0.95f, 0.3f, -0.8f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttack01_2, { 1.05f, 0.3f, -0.7f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
                RegisterFrameEvent(1, 35, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttack01_1, { 0.95f, 0.3f, -0.8f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttack01_2, { 1.05f, 0.3f, -0.7f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
            }
            else if (m_subID == 2)
            {
                RegisterFrameEvent(1, 22, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttack02_1, { -0.85f, 0.3f, -0.1f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttack02_2, { -0.82f, 0.3f, -0.1f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
            }

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_CharkramAttack));
            break;
        }
        case 3:
        {
            RegisterFrameEvent(1, 21, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::DrillAttack1, { 0.05f, 1.1f, 0.0f }, { 3.f, 3.f, 1.f }, { 1, 0, 0 });
                AttachChildBone(eff, 1);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_BreacherAttack));
            break;
        }
        case 4:
        {
            RegisterFrameEvent(1, 30, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::ScytheAttack, { 0.0f, 1.1f, -0.4f }, { 2.f, 2.f, 1.f });
                AttachChildBone(eff, 1);
                eff = m_pEffectManager->Spawn(EffectID::ScytheAttack2, { 0.0f, 1.0f, -0.45f }, { 1.f, 1.f, 1.f });
                AttachChildBone(eff, 1);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_ScytheAttack));
            break;
        }
        case 5:
        {
            RegisterFrameEvent(1, 39, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::AxAttack, { 0.0f, 0.01f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                eff = m_pEffectManager->Spawn(EffectID::AxAttack2, { 0.0f, 0.4f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_ImpactorAttack));
            break;
        }
        case 6:
        {
            RegisterFrameEvent(1, 25, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::AxAttack, { 0.0f, 0.01f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                eff = m_pEffectManager->Spawn(EffectID::AxAttack2, { 0.0f, 0.4f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_CleaverAttack));
            break;
        }
        }
    }
    else if(m_pID == 2)      // Player 2일 경우
    {
        switch (m_weaponID)
        {
        case 1:
        {
            RegisterFrameEvent(1, 36, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::BlasterAttackEnemy, { 0.f, -0.05f, -0.05f }, { 1.f, 1.f, 1.f }, { -1, 0, 0 });
                AttachChildBone(eff, 2);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_BlasterAttack));
            break;
        }
        case 2:
        {
            if (m_subID == 1)
            {
                RegisterFrameEvent(1, 10, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy01_1, { 0.95f, 0.3f, -0.8f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy01_2, { 1.05f, 0.3f, -0.7f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
                RegisterFrameEvent(1, 35, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy01_1, { 0.95f, 0.3f, -0.8f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy01_2, { 1.05f, 0.3f, -0.7f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
            }
            else if (m_subID == 2)
            {
                RegisterFrameEvent(1, 22, [this]() {
                    auto eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy02_1, { -0.85f, 0.3f, -0.1f }, { 2.f, 2.f, 1.f });
                    Attach(eff);
                    eff = m_pEffectManager->Spawn(EffectID::ChakramAttackEnemy02_2, { -0.82f, 0.3f, -0.1f }, { 1.f, 1.f, 1.f });
                    Attach(eff);
                    });
            }

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_CharkramAttack));
            break;
        }
        case 3:
        {
            RegisterFrameEvent(1, 21, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::DrillAttackEnemy1, { 0.05f, 1.1f, 0.0f }, { 3.f, 3.f, 1.f }, { 1, 0, 0 });
                AttachChildBone(eff, 1);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_BreacherAttack));
            break;
        }
        case 4:
        {
            RegisterFrameEvent(1, 30, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::ScytheAttackEnemy, { 0.0f, 1.1f, -0.4f }, { 2.f, 2.f, 1.f });
                AttachChildBone(eff, 1);
                eff = m_pEffectManager->Spawn(EffectID::ScytheAttackEnemy2, { 0.0f, 1.0f, -0.45f }, { 1.f, 1.f, 1.f });
                AttachChildBone(eff, 1);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_ScytheAttack));
            break;
        }
        case 5:
        {
            RegisterFrameEvent(1, 39, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::AxAttackEnemy, { 0.0f, 0.01f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                eff = m_pEffectManager->Spawn(EffectID::AxAttackEnemy2, { 0.0f, 0.4f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_ImpactorAttack));
            break;
        }
        case 6:
        {
            RegisterFrameEvent(1, 25, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::AxAttackEnemy, { 0.0f, 0.01f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                eff = m_pEffectManager->Spawn(EffectID::AxAttackEnemy2, { 0.0f, 0.4f, 0.f }, { 1.f, 1.f, 1.f });
                Attach(eff);
                });

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_CleaverAttack));
            break;
        }
        }
    }

    m_state = PieceAnim::Attack;
}

void UnitPiece::PlayHit()
{
    if (m_animator == nullptr) return;

    bool isChanged = m_animator->Change((m_dir == Direction::Right) ? "hit" : "hitF");
    // 같은 clip("hit")이 이미 current면 Change가 false를 반환한다.
    // 이 경우에도 MoveHit 시점에는 hit를 재생(재시작)해야 하므로
    // idle을 한 번 경유해 hit tick을 리셋한다.
    if (!isChanged)
    {
        m_animator->Change("idle", 0.0f);
        bool retriggered = m_animator->Change((m_dir == Direction::Right) ? "hit" : "hitF", 0.0f);
        if (!retriggered) return;
    }
    m_animator->SetLoop((m_dir == Direction::Right) ? "hit" : "hitF", false);
    m_animator->Play();
    AddAnimState(PieceAnim::Hit);

    AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_Hit));
}

void UnitPiece::PlayMove()
{
    if (m_animator)
    {
        bool isChanged = m_animator->Change("move", (m_weaponID == 6) ? 0.1f : 0.5f);
        if (isChanged)
            m_animator->SetLoop("move", true);
        m_animator->Play();
    }
    for (auto* sub : m_linkedSubPieces)
    {
        if (sub == nullptr) continue;

        sub->PlayMove();
    }

    if (m_subID == 0)
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_Move));

    m_state = PieceAnim::Move;
}

void UnitPiece::PlayRollBack()
{
    m_state = PieceAnim::RollBack;
}

void UnitPiece::PlayRoll()
{
    m_state = PieceAnim::Roll;
}


void UnitPiece::PlayDead(float disappearDisolveDuration)
{
    // 죽음 직후 기존 행동 큐를 정리해 후속 이동/피격 명령이 덮어쓰지 않게 한다.
    ClearQ();

    DisappearDissolve(disappearDisolveDuration);

    if (m_animator == nullptr) return;
    bool isChanged = m_animator->Change((m_dir == Direction::Right) ? "dead" : "deadF");
    if (!isChanged) return;
    m_animator->SetLoop((m_dir == Direction::Right) ? "dead" : "deadF", false);
    m_animator->Play();
    
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_Dead));
}

void UnitPiece::StopMove()
{
    if (m_animator)
    {
        m_animator->Change("idle");
        m_animator->SetLoop("idle", true);
        m_animator->Play();
    }
    // animator가 없는 부모 피스(예: Chakram 루트)도 이동 상태를 반드시 해제해야
    // 다음 큐 명령(공격/피격/이동)이 정상 처리된다.
    m_moveTime = 0.f;
    m_state = PieceAnim::Idle;
}

void UnitPiece::SetTmpColor(Float4 color)
{
    m_vtmpColor = color;
}

void UnitPiece::AddLinkedSubPiece(UnitPiece* subPiece)
{
    if (subPiece == nullptr) return;
    m_linkedSubPieces.push_back(subPiece);
}

void UnitPiece::SetTarget(XMFLOAT3 targetPos, float second)
{
    if (m_state == PieceAnim::Move) return;

    m_fixSpeed = 1 / second;
    m_Target = XMLoadFloat3(&targetPos);
    m_Start = XMLoadFloat3(&this->m_vPos);
    XMVECTOR Dist = XMVectorAbs(XMVector3Length(m_Target - m_Start));
    m_Dist = XMVectorGetX(XMVector3Length(Dist));

    if (m_Dist == 0)
        return;

    PlayMove();
}

Float4 UnitPiece::GetLerpColor(float dt)
{
    float r = m_vtmpColor.x * (1 - dt) + m_flashColor.x * (dt);
    float g = m_vtmpColor.y * (1 - dt) + m_flashColor.y * (dt);
    float b = m_vtmpColor.z * (1 - dt) + m_flashColor.z * (dt);
    float a = m_vtmpColor.w * (1 - dt) + m_flashColor.w * (dt);
    return { r, g, b, a };
}

float UnitPiece::GetLerp(float dt, float start, float end)
{
    return (start * (1 - dt) + end * dt);
}

float UnitPiece::QuadraticGraph(float x)
{
    float result = -(2.f / m_blinkTime * x - 1.f) * (2.f / m_blinkTime * x - 1.f) + 1.f;
    return result;
}

float UnitPiece::linearGraph(float x)
{
    float result = m_linearSlope * x;
    return result;
}

bool UnitPiece::HasAnimState(PieceAnim state) const
{
    return HasThis_Anim(m_state, state);
}

bool UnitPiece::EqualAnimState(PieceAnim state) const
{
    return EqualThis_Anim(m_state, state);
}

void UnitPiece::AddAnimState(PieceAnim state)
{
    m_state = m_state | state;
}

void UnitPiece::RemoveAnimState(PieceAnim state)
{
    m_state = m_state & (~state);
}

void UnitPiece::SetAnimState(PieceAnim state, bool enabled)
{
    if (enabled)
    {
        AddAnimState(state);
    }
    else
    {
        RemoveAnimState(state);
    }
}

void UnitPiece::ClearQ()
{
    while (!m_Q.empty())
    {
        m_Q.pop();
    }
}

PieceAnim operator|(PieceAnim lhs, PieceAnim rhs)
{
    return static_cast<PieceAnim>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}

PieceAnim operator&(PieceAnim lhs, PieceAnim rhs)
{
    return static_cast<PieceAnim>(static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
}

PieceAnim operator~(PieceAnim v)
{
    return static_cast<PieceAnim>(~static_cast<uint16_t>(v));
}

bool HasThis_Anim(PieceAnim a, PieceAnim b)
{
    return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

bool EqualThis_Anim(PieceAnim a, PieceAnim b)
{
    return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b))
        == static_cast<uint16_t>(b);
}
