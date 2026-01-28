#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "Mesh.h"

class UnitDesc;
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
    std::string m_nameS;

    std::wstring unitType;
    std::wstring m_meshpath;

    XMFLOAT3	m_vPos;
    XMFLOAT3	m_vRot;
    XMFLOAT3	m_vScale;
    XMFLOAT3	m_vDir;

    XMFLOAT4X4	m_mWorld;
    XMFLOAT4X4	m_mScale, m_mRot, m_mTrans;

    XMFLOAT3	    m_vPosBk;
    XMFLOAT3	    m_vRotBk;
    XMFLOAT3	    m_vScaleBk;
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

    // 상수버퍼 업데이트할 데이터들
    Update_Data   m_constant;

   std::unique_ptr<MeshNode> m_MeshNode;
   std::vector<Mesh*> m_Meshs; //전체 메쉬 저장용 수명관리는 메쉬노드가 함

    Unit* m_Parent;
    std::unordered_map<uint32_t, Unit*> m_Childs;

protected:
    IRenderer* m_pRenderer = nullptr;
    ITextureManager* m_pTextures = nullptr;
    IInput* m_pInput = nullptr;

public:
    explicit Unit();

    virtual ~Unit();
    //Create는 오브젝트 매니저만 쓰기
    virtual bool  Create(XMFLOAT3 vPos);//일단 호환용으로 냅두고 나중에 무조건 이름 필요한걸로 바꾸는게 나을듯
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);

    virtual bool  Update(float dTime = 0);
    virtual bool  Submit(float dTime = 0);
    bool               LastSubmit(float dTime = 0);      // 이거는 오버라이드 xx

    XMFLOAT3& GetPos() { return m_vPos; }
    XMFLOAT3& GetRot() { return m_vRot; }
    XMFLOAT3& GetScale() { return m_vScale; }

    void SetPos(const XMFLOAT3& pos) { m_vPos = pos; }
    void SetRot(const XMFLOAT3& rot) { m_vRot = rot; }
    void SetScale(const XMFLOAT3& scale) { m_vScale = scale; }

    void SetBackUpTransform() { m_vPos = m_vPosBk; m_vRot = m_vRotBk; m_vScale = m_vScaleBk; }

    virtual void  Backup();

    //메쉬는 대부분 파서가 만들어서 순수 가상함수일 필요없어짐
    virtual bool CreateMesh() { return false; };
    virtual bool CreateMaterial() { return false; };

    virtual void SetMesh(std::unique_ptr<MeshNode>&& mesh);
    void SaveMesh(std::unique_ptr<MeshNode>& node, std::vector<Mesh*>& out);

    void SetEmissiveColor(int num, const XMFLOAT4& color);

    UINT GetMeshNum() { return m_Meshs.size(); }

    void SetTexture(UINT meshindex, TextureUse use, const std::wstring& filepath);

    void SetMaskColor(const XMFLOAT4& col);

    uint32_t GetID() { return m_id; }
    const std::wstring& GetName() const { return m_name; }

    void Attach(Unit* obj);
    void DettachParent();
    void DettachChild(uint32_t id);
    void ClearChild();

    Unit* GetParent() { return m_Parent; }
    std::unordered_map<uint32_t, Unit*>& GetChilds() { return m_Childs; }

    XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }

    void SetMeshPath(const std::wstring& meshpath) { m_meshpath = meshpath; }
    UnitDesc GetDesc();

#ifdef _DEBUG
    virtual void Serialize(); //나중에 상속해서 새로운 오브젝트 만들 때 임구이에 띄우고 싶은거있으면 이 함수 오버라이드하면됌
#endif
};


// 씬 만들떄 처음에 해줘야 되는게
// 크리에이트 할때 오브젝트 매니저 넣기
// 디스트로이 할때 오브젝트 매니저 죽이기
// 로우 포인터 만들어서 오브젝트 매니저로 오브젝트 만들기
// 오브젝트를 만들기 위해서는  Unit에 크리에이트 함수 오버라이드하기
