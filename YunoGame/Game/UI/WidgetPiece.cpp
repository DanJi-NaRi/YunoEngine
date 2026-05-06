#include "pch.h"


#include "PlayQueue.h"
#include "ObjectTypeRegistry.h"
#include "ObjectManager.h"

#include "WidgetPiece.h"

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

WidgetPiece::WidgetPiece(UIFactory& uiFactory) : Widget(uiFactory)
{
    m_name = L"WidgetPiece";
}

//오브젝트 타입.h


WidgetPiece::~WidgetPiece()
{
}


bool WidgetPiece::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

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

    Backup();

    return true;
}


bool WidgetPiece::Update(float dTime)
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
        //case CommandType::Move:
        //{
        //    auto [wx, wy, wz, dir, speed] = tp.mv_p;
        //    SetTarget({ wx, wy, wz }, speed);
        //    SetDir(dir);
        //    m_AnimDone = tp.isDone;        // 슬롯 턴 종료 메세지 보내라
        //    break;
        //}
        //case CommandType::Hit:
        //{
        //    PlayGridQ::Insert(PlayGridQ::Hit_S(m_who, tp.hit.damage1));
        //    if (tp.hit.whichPiece != GamePiece::None)
        //        PlayGridQ::Insert(PlayGridQ::Hit_S(tp.hit.whichPiece, tp.hit.damage2));
        //    break;
        //}
        }
    }

    m_vRot.z = m_yaw;

    //UpdateMatrix();
    
    Widget::Update();

    return true;
}


bool WidgetPiece::Submit(float dTime)
{
    Widget::Submit(dTime);
    return true;
}


bool WidgetPiece::CreateMesh()
{
    VertexStreams streams{};
    streams.flags = VSF_Pos | VSF_UV;
    streams.vtx_count = sizeof(g_tMesh) / sizeof(VERTEX_Pos);
    streams.pos = g_tMesh;
    streams.uv = g_tUV;

    m_defaultMesh = m_pRenderer->CreateMesh(streams, g_tIndex, _countof(g_tIndex));
    if (m_defaultMesh == 0)
        return false;

    return true;
}


bool WidgetPiece::CreateMaterial()
{
    m_Albedo = m_pTextures->LoadTexture2D(L"../Assets/Textures/black.png");

    MaterialDesc md{};
    md.passKey.vs = ShaderId::UIBase;
    md.passKey.ps = ShaderId::UIBase;
    md.passKey.vertexFlags = VSF_Pos | VSF_UV;

    md.passKey.blend = BlendPreset::AlphaBlend;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    md.albedo = m_Albedo;
    md.normal = 0;
    md.orm = 0;

    md.metal = 0;
    md.rough = 0;
    md.ao = 0;

    // 첫번째 머테리얼 생성
    m_defaultMaterial = m_pRenderer->CreateMaterial(md);
    if (m_defaultMaterial == 0)
        return false;

    return true;
}


bool WidgetPiece::UpdateMatrix()
{
    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixRotationZ(m_yaw);
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM;

    if (m_Parent)
        mTM = mScale * mRot * mTrans * m_Parent->GetWorldMatrix();
    else
        mTM = mScale * mRot * mTrans;

    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);

    return true;
}


void WidgetPiece::InsertQ(PGridCmd cmd)
{
    m_Q.push(cmd);
}


void WidgetPiece::SetWho(GamePiece type)
{
    m_who = type;
}


void WidgetPiece::SetDir(Direction dir, bool isAnim)
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


void WidgetPiece::SetTarget(XMFLOAT3 targetPos, float speed)
{
    if (isMoving) return;

    m_fixSpeed = speed;
    m_Target = XMLoadFloat3(&targetPos);
    m_Start = XMLoadFloat3(&m_vPos);
    XMVECTOR Dist = XMVectorAbs(XMVector3Length(m_Target - m_Start));
    m_Dist = XMVectorGetX(XMVector3Length(Dist));

    if (m_Dist == 0)
        return;

    isMoving = true;
}


void WidgetPiece::SetDead()
{
    isDead = true;
}


void WidgetPiece::SendDone()
{

}


void WidgetPiece::ClearQ()
{
    while (!m_Q.empty())
    {
        m_Q.pop();
    }
}

