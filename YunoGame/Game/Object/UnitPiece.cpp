#include "pch.h"

#include "PlayQueue.h"
#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"
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
    // 죽은 상태인지 먼저 확인
    if (isDead && m_deadTime != -1)
    {
        m_deadTime += dTime;
        if (m_deadTime >= m_deathDelay)
        {
            PlayGridQ::Insert(PlayGridQ::Cmd_S(CommandType::Dead, m_who));
            m_deadTime = -1;
        }
        return true;
    }

    // 회전 중이면 회전 처리
    if (isRotating)
    {
        m_rotTime += dTime * m_rotSpeed;
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

    // 이동 중이면 이동 처리
    if (isMoving)
    {
        if (dTime >= 1.f) return true;
        m_AnimTime += dTime / m_Dist * m_speed * m_fixSpeed;

        if (m_AnimTime >= 1.f)
        {
            XMStoreFloat3(&this->m_vPos, m_Target);
            isMoving = false;
            m_AnimTime = 0.f;
            if (m_AnimDone)
            {
                PlayGridQ::Insert(PlayGridQ::Cmd_S(CommandType::Turn_Over, m_who));
                //ClearQ();
                m_AnimDone = false;
            }
        }
        else
        {
            XMVECTOR res = XMVectorLerp(m_Start, m_Target, m_AnimTime);
            XMStoreFloat3(&this->m_vPos, res);
        }
    }

    UpdateFlash(dTime);

    // 애니메이션이 끝나면 하나씩 빼가게 하기
    while (!m_Q.empty() && !isMoving && !isRotating)
    {
        auto tp = m_Q.front();
        m_Q.pop();
        switch (tp.cmdType)
        {
        case CommandType::Move:
        {
            auto [wx, wy, wz, dir, speed] = tp.mv_p;
            SetTarget({ wx, wy, wz }, speed);
            SetDir(dir);
            m_AnimDone = tp.isDone;        // 슬롯 턴 종료 메세지 보내라
            break;
        }
        case CommandType::Hit:
        {
            SetFlashColor({0,0,0,1},1,0.3f);
            PlayGridQ::Insert(PlayGridQ::Hit_S(m_who));
            if (tp.hit.whichPiece != GamePiece::None)
                PlayGridQ::Insert(PlayGridQ::Hit_S(tp.hit.whichPiece));
            break;
        }
        }
    }

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


bool UnitPiece::UpdateMatrix()
{
    XMMATRIX mScale = XMMatrixScaling(m_vScale.x,m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationY(m_yaw);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

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

void UnitPiece::UpdateFlash(float dt)
{
    if (isFlashing)
    {
        // 속도 조절 가능
        m_flashTime += dt;

        // 0에서 시작해 1까지 올라갔다 다시 내려오는 부드러운 곡선
        float btw0_1 = Graph(m_flashTime);

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


void UnitPiece::InsertQ(PGridCmd cmd)
{
    m_Q.push(cmd);
}


void UnitPiece::SetWho(GamePiece type)
{
    m_who = type;
}


void UnitPiece::SetDir(Direction dir, bool isAnim)
{
    switch (dir)
    {
    case Direction::Same:
        return;
        break;
    case Direction::Right:
        m_targetYaw = atan2(-1, 0);
        break;
    case Direction::Left:
        m_targetYaw = atan2(1, 0);
        break;
    }
    if (m_targetYaw == m_yaw) return;

    m_startYaw = m_yaw;
    isRotating = isAnim;

    m_yaw = (isAnim) ? m_yaw : m_targetYaw;
}

void UnitPiece::SetFlashColor(Float4 color, int count, float blinkTime)
{
    m_flashColor = color;
    m_count = count;
    m_blinkTime = blinkTime;
    isFlashing = true;
}


void UnitPiece::SetTarget(XMFLOAT3 targetPos, float speed)
{
    if (isMoving) return;

    m_fixSpeed = speed;
    m_Target = XMLoadFloat3(&targetPos);
    m_Start = XMLoadFloat3(&this->m_vPos);
    XMVECTOR Dist = XMVectorAbs(XMVector3Length(m_Target - m_Start));
    m_Dist = XMVectorGetX(XMVector3Length(Dist));

    if (m_Dist == 0)
        return;

    isMoving = true;
}


void UnitPiece::SetDead()
{
    isDead = true;
}

void UnitPiece::SetTmpColor(Float4 color)
{
    m_vtmpColor = color;
}


void UnitPiece::SendDone()
{

}


void UnitPiece::ClearQ()
{
    while (!m_Q.empty())
    {
        m_Q.pop();
    }
}

Float4 UnitPiece::GetLerpColor(float dt)
{
    float r = m_vtmpColor.x * (1 - dt) + m_flashColor.x * (dt);
    float g = m_vtmpColor.y * (1 - dt) + m_flashColor.y * (dt);
    float b = m_vtmpColor.z * (1 - dt) + m_flashColor.z * (dt);
    float a = m_vtmpColor.w * (1 - dt) + m_flashColor.w * (dt);
    return { r, g, b, a };
}

float UnitPiece::Graph(float x)
{
    float result = -(2.f / m_blinkTime * x - 1.f) * (2.f / m_blinkTime * x - 1.f) + 1.f;

    return result;
}
