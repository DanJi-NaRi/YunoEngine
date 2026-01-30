#pragma once
#include "Unit.h"
#include "Widget.h"
#include "GridFactory.h"

template <typename T>
class GridLine : public T
{
public:
    GridLine();
    template<typename... Args>
    explicit GridLine(Args&&... args)
        : T(std::forward<Args>(args)...) {
    }
    virtual ~GridLine();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
private:
    bool CreateMesh();      // 메시 생성 (한 번만)
    bool CreateMaterial();  // 머테리얼 생성 (한 번만)

private:
    // 라인 그리기용 버텍스
    std::vector<VERTEX_Pos> m_lineVtx;
};

extern template class GridLine<Unit>;
extern template class GridLine<Widget>;

// Widget일 경우
template<>
bool GridLine<Widget>::CreateMesh();

