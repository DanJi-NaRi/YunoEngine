#include "pch.h"

#include "PlayQueue.h"

#include "Piece.h"
#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

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

VERTEX_Pos g_tMesh[] =
{
    { 0.0f, 0.5f, 0.0f },    // 아래쪽 꼭짓점
    { 0.5f, -0.4f, 0.0f },   // 오른쪽 위 꼭짓점
    {-0.5f, -0.4f, 0.0f }    // 왼쪽 위 꼭짓점
};

VERTEX_UV g_tUV[] =
{
    {0.0, 0.0},
    {1.0, 0.0},
    {0.5, 1.0}
};

INDEX g_tIndex[] =
{
    {0, 1, 2}
};

//오브젝트 타입.h

//namespace {
//    struct AutoReg_Piece
//    {
//        AutoReg_Piece()
//        {
//            ObjectTypeRegistry::Instance().Register(L"Piece", [](ObjectManager& om, const UnitDesc& d) { om.CreateObjectInternal<Piece<Unit>>(d); });
//        }
//    } s_reg_Piece;
//}

template class Piece<Unit>;
template class Piece<Widget>;

template<typename T>
Piece<T>::Piece()
{
    this->m_name = L"Piece";
}

template<typename T>
Piece<T>::~Piece()
{
}

template<typename T>
bool Piece<T>::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    T::Create(name, id, vPos);

    if (!this->m_pInput || !this->m_pRenderer || !this->m_pTextures)
        return false;
    if (!CreateMesh())
        return false;
    if (!CreateMaterial())
        return false;

    auto mesh = std::make_unique<Mesh>();


    XMMATRIX i = XMMatrixIdentity();
    mesh->Create(this->m_defaultMesh, this->m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));

    this->m_MeshNode = std::make_unique<MeshNode>();

    this->m_MeshNode->m_Meshs.push_back(std::move(mesh));

    T::Backup();

    return true;
}

template<typename T>
bool Piece<T>::Update(float dTime)
{
    // 죽은 상태인지 먼저 확인
    if (isDead && m_deadTime != -1)
    {
        m_deadTime += dTime;
        if (m_deadTime >= m_deathDelay)
        {
            PlayGridQ::Insert(PlayGridQ::Cmd_S(CommandType::Dead,m_who));
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
                ClearQ();
                m_AnimDone = false;
            }
        }
        else
        {
            XMVECTOR res = XMVectorLerp(m_Start, m_Target, m_AnimTime);
            XMStoreFloat3(&this->m_vPos, res);
        }
    }

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
                PlayGridQ::Insert(PlayGridQ::Hit_S(m_who, tp.hit.damage1));
                if (tp.hit.whichPiece != GamePiece::None)
                    PlayGridQ::Insert(PlayGridQ::Hit_S(tp.hit.whichPiece, tp.hit.damage2));
                break;
            }
        }  
    }

    UpdateMatrix();

    return true;
}

template<typename T>
bool Piece<T>::Submit(float dTime)
{
    T::Submit(dTime);
    return true;
}

template<typename T>
bool Piece<T>::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_Nrm | VSF_UV;
    streams.vtx_count = sizeof(g_cubeMesh_2) / sizeof(VERTEX_Pos);
    streams.pos = g_cubeMesh_2;
    streams.nrm = g_cubeNrm_2;
    streams.uv = g_cubeUV_2;

    T::m_defaultMesh = T::m_pRenderer->CreateMesh(streams, g_cubeIndex_2, _countof(g_cubeIndex_2));
    if (T::m_defaultMesh == 0)
        return false;

    return true;
}

template<typename T>
bool Piece<T>::CreateMaterial()
{
    this->m_Albedo = this->m_pTextures->LoadTexture2D(L"../Assets/Textures/foxUV.png");

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

template<typename T>
bool Piece<T>::UpdateMatrix()
{
    XMMATRIX mScale = XMMatrixScaling(this->m_vScale.x, this->m_vScale.y, this->m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationY(m_yaw);
    XMMATRIX mTrans = XMMatrixTranslation(this->m_vPos.x, this->m_vPos.y, this->m_vPos.z);

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

template<typename T>
void Piece<T>::InsertQ(PGridCmd cmd)
{
    m_Q.push(cmd);
}

template<typename T>
void Piece<T>::SetWho(GamePiece type)
{
    m_who = type;
}

template<typename T>
void Piece<T>::SetDir(Direction dir, bool isAnim)
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
    m_startYaw = m_yaw;
    isRotating = isAnim;

    m_yaw = (isAnim) ? m_yaw : m_targetYaw;
}

template<typename T>
void Piece<T>::SetTarget(XMFLOAT3 targetPos, float speed)
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

template<typename T>
void Piece<T>::SetDead()
{
    isDead = true;
}

template<typename T>
void Piece<T>::SendDone()
{
    
}

template<typename T>
void Piece<T>::ClearQ()
{
    while (!m_Q.empty())
    {
        m_Q.pop();
    }
}

template<>
bool Piece<Widget>::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_UV;
    streams.vtx_count = sizeof(g_tMesh) / sizeof(VERTEX_Pos);
    streams.pos = g_tMesh;
    streams.uv = g_tUV;

    this->m_defaultMesh = this->m_pRenderer->CreateMesh(streams, g_tIndex, _countof(g_tIndex));
    if (this->m_defaultMesh == 0)
        return false;

    return true;
}

template<>
bool Piece<Widget>::CreateMaterial()
{
    this->m_Albedo = this->m_pTextures->LoadTexture2D(L"../Assets/Textures/black.png");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::UIBase;
    md.passKey.ps = ShaderId::UIBase;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;

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


template<>
bool Piece<Widget>::UpdateMatrix()
{
    XMMATRIX mScale = XMMatrixScaling(this->m_vScale.x, this->m_vScale.y, this->m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationZ(m_yaw);
    XMMATRIX mTrans = XMMatrixTranslation(this->m_vPos.x, this->m_vPos.y, this->m_vPos.z);

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
