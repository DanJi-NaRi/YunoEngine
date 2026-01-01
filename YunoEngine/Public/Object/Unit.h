#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"


// 모든 메쉬는 CW 시계방향 정점이 앞면임
/* 예시)
01
23   >> 인덱스 012, 213 이 앞면 쿼드
*/
class Unit 
{
protected:

    XMFLOAT3	m_vPos;
    XMFLOAT3	m_vRot;
    XMFLOAT3	m_vScale;
    XMFLOAT3	m_vDir;

    XMFLOAT4X4	m_mWorld;
    XMFLOAT4X4	m_mScale, m_mRot, m_mTrans;



    XMFLOAT3	m_vPosBk;
    XMFLOAT3	m_vRotBk;
    XMFLOAT3	m_vScaleBk;
    XMFLOAT3 	m_vDirBk;

    int			m_Hp;
    int			m_Mana;
    int			m_Lv;
    int			m_Exp;


    RenderItem      m_renderItem;
    MeshHandle      m_mesh;
    MaterialHandle  m_material;
    TextureHandle   m_Albedo;
    TextureHandle   m_Normal;
    TextureHandle   m_Orm;

public:
    explicit Unit();

    virtual ~Unit();

    virtual bool  Create(XMFLOAT3 vPos);
    virtual bool  Update(float dTime = 0);
    virtual bool  Submit();

    virtual void  Backup();

    virtual bool CreateMesh() = 0;
    virtual bool CreateMaterial() = 0;
};