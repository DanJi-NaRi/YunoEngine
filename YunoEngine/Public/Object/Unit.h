#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"

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

public:
    explicit Unit();

    virtual ~Unit();

    virtual bool  Create(XMFLOAT3 vPos);
    virtual bool  Update(float dTime = 0);
    virtual bool  Submit();

    virtual void  Backup();

};