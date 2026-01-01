#include "pch.h"

#include "Unit.h"


Unit::Unit()
{
    m_vPos = XMFLOAT3(0, 0, 0);
    m_vRot = XMFLOAT3(0, 0, 0);
    m_vScale = XMFLOAT3(1, 1, 1);
    XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mRot, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mScale, XMMatrixIdentity());
    XMStoreFloat4x4(&m_mTrans, XMMatrixIdentity());

    m_vDir = XMFLOAT3(0, 0, 1);

    m_Hp = 0;
    m_Mana = 0;
    m_Lv = 0;
    m_Exp = 0;


    m_mesh = 0;
    m_material = 0;
    m_Albedo = 0;
    m_Normal = 0;
    m_Orm = 0;
}

Unit::~Unit()
{
}

bool Unit::Create(XMFLOAT3 vPos)
{
    m_vPos = vPos;
    m_vRot = XMFLOAT3(0, 0, 0);
    m_vScale = XMFLOAT3(1, 1, 1);

    Unit::Update();

    return true;
}

bool Unit::Update(float dTime)
{

    XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX mRot = XMMatrixIdentity();
    XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX mTM = mScale * mRot * mTrans;


    XMStoreFloat4x4(&m_mScale, mScale);
    XMStoreFloat4x4(&m_mRot, mRot);
    XMStoreFloat4x4(&m_mTrans, mTrans);
    XMStoreFloat4x4(&m_mWorld, mTM);


    return true;
}

bool Unit::Submit()
{
    m_renderItem.meshHandle = m_mesh;
    m_renderItem.materialHandle = m_material;
    m_renderItem.mWorld = m_mWorld;

    YunoEngine::GetRenderer()->Submit(m_renderItem);

    return true;
}

void Unit::Backup()
{
    m_vPosBk = m_vPos;
    m_vRotBk = m_vRot;
    m_vScaleBk = m_vScale;
    m_vDirBk = m_vDir;
}

