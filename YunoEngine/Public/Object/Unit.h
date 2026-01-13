#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "Mesh.h"


// 모든 메쉬는 CW 시계방향 정점이 앞면임
/* 예시)
01
23   >> 인덱스 012, 213 이 앞면 쿼드
*/
class Unit 
{
protected:
    uint32_t m_id;
    std::wstring m_name;

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

    float       m_time;


    RenderItem      m_renderItem;
    MeshHandle      m_defaultMesh;
    MaterialHandle  m_defaultMaterial;
    TextureHandle   m_Albedo;
    TextureHandle   m_Normal;
    TextureHandle   m_Orm;

    std::vector<std::unique_ptr<Mesh>> m_Meshs;

    Unit* m_Parent;
    std::unordered_map<uint32_t, Unit*> m_Childs;

public:
    explicit Unit();

    virtual ~Unit();
    //Create는 오브젝트 매니저만 쓰기
    virtual bool  Create(XMFLOAT3 vPos);//일단 호환용으로 냅두고 나중에 무조건 이름 필요한걸로 바꾸는게 나을듯
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);

    virtual bool  Update(float dTime = 0);
    virtual bool  Submit(float dTime = 0);
    bool          LastSubmit(float dTime = 0);      // 이거는 오버라이드 xx

    virtual void  Backup();

    //메쉬는 대부분 파서가 만들어서 순수 가상함수일 필요없어짐
    virtual bool CreateMesh() { return false; };
    virtual bool CreateMaterial() { return false; };

    virtual void SetMesh(std::unique_ptr<Mesh>&& mesh);

    uint32_t GetID() { return m_id; }
    const std::wstring& GetName() { return m_name; }

    void Attach(Unit* obj);
    void DettachParent();
    void DettachChild(uint32_t id);
    void ClearChild();

    XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }
};


// 씬 만들떄 처음에 해줘야 되는게
// 크리에이트 할때 오브젝트 매니저 넣기
// 디스트로이 할때 오브젝트 매니저 죽이기
// 로우 포인터 만들어서 오브젝트 매니저로 오브젝트 만들기
// 오브젝트를 만들기 위해서는  Unit에 크리에이트 함수 오버라이드하기
