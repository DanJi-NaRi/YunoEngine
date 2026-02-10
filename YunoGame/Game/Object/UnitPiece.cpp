#include "pch.h"

#include "PlayQueue.h"
#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"
#include "EffectManager.h"
#include "UnitPiece.h"

float scaleAdjust = 0.2f;
VERTEX_Pos g_cubeMesh_2[] =
{
    { -7.0f * scaleAdjust,  0.0f, 0.0f},
    {0.0f, 14.0f * scaleAdjust, -1.f * scaleAdjust},
    {7.0f * scaleAdjust,  0.0f, 0.0f},

    {-1.0f * scaleAdjust,  6.0f * scaleAdjust, -0.5f * scaleAdjust},
    {-2.0f * scaleAdjust,  0.0f, -2.0f * scaleAdjust},
    {-3.0f * scaleAdjust,  6.0f * scaleAdjust, -0.5f * scaleAdjust},

    {3.0f * scaleAdjust,  6.0f * scaleAdjust, -0.5f * scaleAdjust},
    {2.0f * scaleAdjust,  0.0f, -2.f * scaleAdjust},
    {1.0f * scaleAdjust,  6.0f * scaleAdjust, -0.5f * scaleAdjust},

    {0.0f,  17.0f * scaleAdjust, 0.f},
    {0.0f,  9.0f * scaleAdjust, -3.f * scaleAdjust},
    {-6.0f * scaleAdjust,  22.0f * scaleAdjust, 2.f * scaleAdjust},

    {6.0f * scaleAdjust,  22.0f * scaleAdjust, 2.f * scaleAdjust},
    {0.0f,  9.0f * scaleAdjust, -3.f * scaleAdjust},
    {0.0f,  17.0f * scaleAdjust, 0.f},

    {0.0f,  0.0f, scaleAdjust * scaleAdjust},
    {-15.0f * scaleAdjust,  6.0f * scaleAdjust, 5.f * scaleAdjust},
    {-12.0f * scaleAdjust,  8.0f * scaleAdjust, 5.f * scaleAdjust},

    {0.0f,  0.0f, scaleAdjust * scaleAdjust},
    {-10.0f * scaleAdjust,  12.0f * scaleAdjust, 5.f * scaleAdjust},
    {-7.0f * scaleAdjust,  13.0f * scaleAdjust, 5.f * scaleAdjust},

    {0.0f,  0.0f, scaleAdjust * scaleAdjust},
    {-1.5f * scaleAdjust,  15.0f * scaleAdjust, 5.f * scaleAdjust},
    {1.5f * scaleAdjust,  15.0f * scaleAdjust, 5.f * scaleAdjust},

    {0.0f,  0.0f, scaleAdjust * scaleAdjust},
    {7.f * scaleAdjust,  13.0f * scaleAdjust, 5.f * scaleAdjust},
    {10.f * scaleAdjust,  12.0f * scaleAdjust, 5.f * scaleAdjust},

    {0.0f,  0.0f, scaleAdjust * scaleAdjust},
    {12.f * scaleAdjust,  8.0f * scaleAdjust, 5.f * scaleAdjust},
    {15.f * scaleAdjust,  6.0f * scaleAdjust, 5.f * scaleAdjust}

};

VERTEX_Nrm g_cubeNrm_2[] =
{
    {0.0f, 0.071f, -0.997f},
    {0.0f, 0.071f, -0.997f},
    {0.0f, 0.071f, -0.997f},

    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},

    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},

    {-0.444f, 0.148f, -0.884f},
    {-0.444f, 0.148f, -0.884f},
    {-0.444f, 0.148f, -0.884f},

    {0.444f, 0.148f, -0.884f},
    {0.444f, 0.148f, -0.884f},
    {0.444f, 0.148f, -0.884f},

    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},

    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},

    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},

    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},

    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f},
    {0.0f, 0.0f, -1.f}

};

VERTEX_UV g_cubeUV_2[] =
{
    {0.525f, 0.988f},
    {0.758f, 0.521f},
    {0.992f, 0.988f},

    {0.037f, 0.004f},
    {0.068f, 0.211f},
    {0.102f, 0.004f},

    {0.102f, 0.004f},
    {0.068f, 0.211f},
    {0.037f, 0.004f},

    {0.248f, 0.738f},
    {0.248f, 0.992f},
    {0.0469f, 0.561f},

    {0.0469f, 0.561f},
    {0.248f, 0.992f},
    {0.248f, 0.738f},

    {0.455f, 0.451f},
    {0.166f, 0.002f},
    {0.275f, 0.002f},

    {0.529f, 0.484f},
    {0.346f, 0.002f},
    {0.447f, 0.002f},

    {0.588f, 0.516f},
    {0.537f, 0.002f},
    {0.637f, 0.002f},

    {0.637f, 0.484f},
    {0.723f, 0.002f},
    {0.822f, 0.002f},

    {0.707f, 0.455f},
    {0.883f, 0.002f},
    {0.996f, 0.002f}

};

INDEX g_cubeIndex_2[] =
{
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {9, 10, 11},    // 왼쪽 머리
    {12, 13, 14},   // 오른쪽 머리
    {15, 16, 17},
    {18, 19, 20},
    {21, 22, 23},
    {24, 25, 26},
    {27, 28, 29}

};

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

    //if (!m_pInput || !m_pRenderer || !m_pTextures)
    //    return false;
    //if (!CreateMesh())
    //    return false;
    //if (!CreateMaterial())
    //    return false;

    //auto mesh = std::make_unique<Mesh>();


    //XMMATRIX i = XMMatrixIdentity();
    //mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));

    //m_MeshNode = std::make_unique<MeshNode>();

    //m_MeshNode->m_Meshs.push_back(std::move(mesh));

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

    UpdateRot(dTime);
    UpdateMove(dTime);
    UpdateFlash(dTime);
    m_curRotOffset = (isAttacking) ? 0.f : (m_dir == Direction::Right) ? -m_rotOffset : m_rotOffset;
    m_curMoveOffset = (isAttacking) ? 0.f : m_moveOffset;
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


bool UnitPiece::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_cubeMesh_2) / sizeof(VERTEX_Pos);
    streams.pos = g_cubeMesh_2;
    streams.nrm = g_cubeNrm_2;
    streams.uv = g_cubeUV_2;

    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_cubeIndex_2, _countof(g_cubeIndex_2));
    if (m_defaultMesh == 0)
        return false;

    return true;
}


bool UnitPiece::CreateMaterial()
{
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/foxUV.png");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::Basic;
    md.passKey.ps = ShaderId::Basic;
    md.passKey.vertexFlags = VSF_Pos | VSF_Nrm | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    md.albedo = this->m_Albedo;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    // 첫번째 머테리얼 생성
    this->m_defaultMaterial = this->m_pRenderer->CreateMaterial(md);
    if (this->m_defaultMaterial == 0)
        return false;

    return true;
}

bool UnitPiece::CheckDead(float dt)
{
    if (!isDead)  return false;
    if (isDeadQueued) return true;

    // 다 사라지고 난 뒤에만 시스템에 삭제를 요청한다.
    if (m_dissolveAmount == 1)
    {
        PlayGridQ::Insert(PlayGridQ::Cmd_S(CommandType::Dead, m_who));
        isDeadQueued = true;
    }

    return true;
}

void UnitPiece::CheckMyQ()
{
    if (isDead) return;

    // 애니메이션이 끝나면 하나씩 빼가게 하기
    while (!m_Q.empty() && !isMoving && !isRotating && !isHitting && !isAttacking && !isRollingBack && !isRolling)
    {
        auto tp = m_Q.front();
        m_Q.pop();
        switch (tp.cmdType)
        {
        case CommandType::Rotate:
        {
            SetDir(tp.rot_p.dir, true, 0.5f);
            break;
        }
        case CommandType::Move:
        {
            auto [wx, wy, wz, second] = tp.mv_p;
            PlayMove({ wx, wy, wz }, second);

            break;
        }
        case CommandType::MoveHit:
        {
            if (tp.hit_p.amIdead)
                PlayDead(tp.hit_p.disappearDissolveDuration);
            else
                PlayHit();

            PlayGridQ::Insert(PlayGridQ::Hit_S(tp.hit_p.whichPiece));
            break;
        }
        case CommandType::Attack:
        {
            PlayRollBack();
            //PlayAttack();
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

void UnitPiece::UpdateRot(float dt)
{
    if (!isRotating)    return;
    
    m_rotTime += dt * m_rotSpeed;
    if (m_rotTime >= 1.f)
    {
        m_yaw = m_targetYaw;
        m_rotTime = 0.f;
        isRotating = false;
    }
    else
    {
        m_yaw = m_startYaw * (1 - m_rotTime) + m_targetYaw * m_rotTime;
    }
    
}

void UnitPiece::UpdateMove(float dt)
{
    if (!isMoving)   return;

    m_moveTime += dt / m_Dist * m_speed * m_fixSpeed;

    if (m_moveTime >= 1.f)
    {
        XMStoreFloat3(&this->m_vPos, m_Target);
        
        if (m_animator)
        {
            m_animator->Change("idle");
            m_animator->SetLoop("idle", true);      // 여기서 터지면 idle 애니메이션이 없는 것임으로 생성단계에 클립을 넣어야함!
            m_animator->Play();
        }
        isMoving = false;
        m_moveTime = 0.f;
    }
    else
    {
        XMVECTOR res = XMVectorLerp(m_Start, m_Target, m_moveTime);
        XMStoreFloat3(&this->m_vPos, res);
    } 
}

void UnitPiece::UpdateFlash(float dt)
{
    if (isFlashing)
    {
        // 속도 조절 가능
        m_flashTime += dt;

        // 0에서 시작해 1까지 올라갔다 다시 내려오는 부드러운 곡선
        float btw0_1 = QuadraticGraph(m_flashTime);

        // btw0_1을 기반으로 색상 보간
        Float4 fc = GetLerpColor(btw0_1);
        SetMaskColor({ fc.x, fc.y, fc.z, fc.w });
        //SetEmissiveColor(2, { fc.x, fc.y, fc.z, fc.w });

        if (m_flashTime >= m_blinkTime)
        {
            m_count--;
            m_flashTime = 0.f;
        }
        if (m_count == 0)
        {
            // 종료 시 원래 색 복원
            SetMaskColor({ m_vtmpColor.x, m_vtmpColor.y, m_vtmpColor.z, m_vtmpColor.w });
            //SetEmissiveColor(2, { m_vtmpColor.x, m_vtmpColor.y, m_vtmpColor.z, m_vtmpColor.w });
            m_flashTime = 0.f;
            isFlashing = false;
        }

    }
}

void UnitPiece::UpdateHit(float dt)
{
    if (!isHitting)  return;

    if (m_animator)
    {
        bool isPlaying = m_animator->isPlaying();
        if (!isPlaying)
        {
            m_animator->Change("idle");
            m_animator->SetLoop("idle", true);
            m_animator->Play();
            isHitting = false;

            //// 피격 애니메이션 끝나면 system에게 기물 생사여부 체크하도록 함.
            //PGridCmd cmd{ CommandType::Hit };
            //cmd.hit.whichPiece = GamePiece::None;
            //m_Q.push(cmd);
        }
    }
}

void UnitPiece::UpdateDissolve(float dt)
{
    if (!isDissolving) return;

    m_dissolveTime += dt;

    if (m_dissolveTime >= m_dissolveDuration)
    {
        m_dissolveAmount = m_startDissolveAmount + linearGraph(m_dissolveDuration);
        m_dissolveTime = 0.f;
        isDissolving = false;
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
    if (!isAttacking) return;

    if (m_animator == nullptr)
    {
        isAttacking = false;
        return;
    }

    bool isPlaying = m_animator->isPlaying();

    if (!isPlaying)
    {
        m_animator->Change("idle");
        m_animator->Play();
        isAttacking = false;
    }
}

void UnitPiece::UpdateRollingOrBack(float dt)
{
    if (isRollingBack)
    {
        m_rollTime += dt * m_rollorbackSpeed;

        m_curMoveOffset = GetLerp(m_rollTime, m_moveOffset, 0);
        m_curRotOffset = GetLerp(m_rollTime, m_rotOffset, 0);
        
        if (m_rollTime >= 1.f)
        {
            m_curMoveOffset = 0;
            m_curRotOffset = 0;
            m_rollTime = 0;
            isRollingBack = false;
            
            PlayAttack();
            isRolling = true;
        }
    }
    else if (isRolling && !isAttacking)
    {
        m_rollTime += dt * m_rollorbackSpeed;

        m_curMoveOffset = GetLerp(m_rollTime, 0, m_moveOffset);
        m_curRotOffset = GetLerp(m_rollTime, 0, m_rotOffset);

        if (m_rollTime >= 1.f)
        {
            m_curMoveOffset = m_moveOffset;
            m_curRotOffset = m_rotOffset;
            m_rollTime = 0;
            isRolling = false;
        }
    }
}


void UnitPiece::InsertQ(PGridCmd cmd)
{
    m_Q.push(cmd);
}


void UnitPiece::SetWho(GamePiece type, Team team, uint8_t weaponID, uint8_t subID)
{
    m_who = type;
    m_team = team;
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
    isRotating = isAnim;
    m_rotSpeed = 1 / second;

    m_yaw = (isAnim) ? m_yaw : m_targetYaw;
}

void UnitPiece::SetFlashColor(Float4 color, int count, float blinkTime)
{
    m_flashColor = color;
    m_count = count;
    m_blinkTime = blinkTime;
    isFlashing = true;
}

void UnitPiece::AppearDissolve(float dissolveTime)
{
    m_linearSlope = -(1.f / dissolveTime);
    m_dissolveDuration = dissolveTime;
    m_startDissolveAmount = m_dissolveAmount;
    isDissolving = true;
}

void UnitPiece::DisappearDissolve(float dissolveTime)
{
    m_linearSlope = 1.f / dissolveTime;
    m_dissolveDuration = dissolveTime;
    m_startDissolveAmount = m_dissolveAmount;
    isDissolving = true;
    isDead = true;
    isDeadQueued = false;
}

void UnitPiece::PlayAttack()
{
    if (m_animator == nullptr) return;
    bool isChanged = m_animator->Change("attack");
    if (!isChanged) return;
    m_animator->SetLoop("attack", false);
    m_animator->Play();

    // 이펙트
    if (m_team == Team::Ally)       // 아군
    {
        switch (m_weaponID)
        {
        case 1:
        {
            RegisterFrameEvent(1, 36, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::BlasterAttack, { 0.f, -0.05f, -0.05f }, { 1.f, 1.f, 1.f }, { -1, 0, 0 });
                AttachChildBone(eff, 2);
                });
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
            break;
        }
        case 3:
        {
            RegisterFrameEvent(1, 21, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::DrillAttack1, { 0.05f, 1.1f, 0.0f }, { 3.f, 3.f, 1.f }, { 1, 0, 0 });
                AttachChildBone(eff, 1);
                });
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
            break;
        }
        }
    }
    else if(m_team == Team::Enemy)      // 적군
    {
        switch (m_weaponID)
        {
        case 1:
        {
            RegisterFrameEvent(1, 36, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::BlasterAttackEnemy, { 0.f, -0.05f, -0.05f }, { 1.f, 1.f, 1.f }, { -1, 0, 0 });
                AttachChildBone(eff, 2);
                });
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
            break;
        }
        case 3:
        {
            RegisterFrameEvent(1, 21, [this]() {
                auto eff = m_pEffectManager->Spawn(EffectID::DrillAttackEnemy1, { 0.05f, 1.1f, 0.0f }, { 3.f, 3.f, 1.f }, { 1, 0, 0 });
                AttachChildBone(eff, 1);
                });
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
            break;
        }
        }
    }

    isAttacking = true;
}

void UnitPiece::PlayHit()
{
    //SetFlashColor(color, count, blinkTime);
    if (m_animator == nullptr) return;
    bool isChanged = m_animator->Change("hit");
    if (!isChanged) return;
    m_animator->SetLoop("hit", false);
    m_animator->Play();
    isHitting = true;
}

void UnitPiece::PlayMove(XMFLOAT3 targetPos, float second)
{
    if (isMoving) return;

    m_fixSpeed = 1 / second;
    m_Target = XMLoadFloat3(&targetPos);
    m_Start = XMLoadFloat3(&this->m_vPos);
    XMVECTOR Dist = XMVectorAbs(XMVector3Length(m_Target - m_Start));
    m_Dist = XMVectorGetX(XMVector3Length(Dist));
    
    if (m_animator)
    {
        bool isChanged = m_animator->Change("move");
        if (isChanged)
            m_animator->SetLoop("move", true);
        m_animator->Play();
    }

    if (m_Dist == 0)
        return;

    isMoving = true;
}

void UnitPiece::PlayRollBack(float seconds)
{
    //m_rollorbackSpeed = 1.f / seconds;
    isRollingBack = true;

    isRolling = false;
}

void UnitPiece::PlayRoll(float seconds)
{
    m_rollorbackSpeed = 1.f / seconds;
    isRolling = true;
}


void UnitPiece::PlayDead(float disappearDisolveDuration)
{
    // 죽음 직후 기존 행동 큐를 정리해 후속 이동/피격 명령이 덮어쓰지 않게 한다.
    ClearQ();
    isMoving = false;
    isRotating = false;
    isAttacking = false;
    isHitting = false;

    DisappearDissolve(disappearDisolveDuration);

    if (m_animator == nullptr) return;
    bool isChanged = m_animator->Change("dead");
    if (!isChanged) return;
    m_animator->SetLoop("dead", false);
    m_animator->Play();
}

void UnitPiece::SetTmpColor(Float4 color)
{
    m_vtmpColor = color;
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

void UnitPiece::ClearQ()
{
    while (!m_Q.empty())
    {
        m_Q.pop();
    }
}
