#pragma once

#include "Unit.h"

// 각 오브젝트 헤더 파일 최상단에는 Unit 헤더가 있어야됨
class Test_Unit : public Unit
{
public:
    explicit Test_Unit();
    virtual ~Test_Unit();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    void SetId(int id) { m_unitId = id; };
private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

    int m_unitId = 0;
    //추가 머테리얼
    MaterialHandle m_addMaterial1 = 0;
    MaterialHandle m_addMaterial2 = 0;
    MaterialHandle m_addMaterial3 = 0;
    MaterialHandle m_addMaterial4 = 0;
    MaterialHandle m_addMaterial5 = 0;
    MaterialHandle m_addMaterial6 = 0;
};

