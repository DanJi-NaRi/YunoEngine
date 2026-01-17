#pragma once
#include "Widget.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class Image : public Widget {
    public:
    explicit Image();
    virtual ~Image();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

    //추가 머테리얼 - 필요할까?
    //MaterialHandle m_addMaterial = 0;
};
