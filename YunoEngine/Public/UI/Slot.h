#pragma once
#include "Image.h"

struct SnapPoint {
    XMFLOAT2 snapPos; // 스냅 위치 : 기본적으로 slot과 1:1이겠지만, 슬롯이 여러 스냅포인트를 가진 경우 달라질 수 있다.
    RECT snapRange;   // 스냅 검사 Rect : 위젯이 해당 Rect와 AABB가 통과되면, snapPos로 스냅한다.
    float snapPadding; // 범위 보정치 : Rect 감지 범위에 padding만큼 추가 보정을 한다.
    WidgetClass snapTargetClass; // 스냅 조건
    Widget* pSnapOwner; // 스냅한 위젯. // 사실 이걸로 IsSnapped 역할도 한다.
    //id를 알게 하면 더 최적화될수도?
public:
    bool IsSnapped() { return (pSnapOwner); }
};

// 스냅 포인트가 탑재된 위젯.
// SnapPoint만 필요하다면 SnapPoint타입 멤버를 추가해서 활용해도 되지만...
// '카드 빈자리' 등의 스프라이트가 필요하다면, 해당 클래스를 상속하자.
class Slot : public Widget
{
public:
    Slot(UIFactory& uiFactory);
    virtual ~Slot();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    virtual bool UpdateTransform(float dTime = 0) override; //snapPoint 갱신 추가를 위해 override
    bool Submit(float dTime = 0) override;


    virtual WidgetType GetWidgetType() override { return WidgetType::Slot; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Slot; }

    // 스냅 포인트
    void UpdateSnapPoint(float dTime = 0);
    void SetSnapPoint(float padding, WidgetClass target); // padding : 보정치, target : 받아들일 클래스 타입
    SnapPoint* GetSnapPoint() { return &m_snapPoint; }

    bool IsSnapped() { return m_snapPoint.IsSnapped(); }
    bool ReleaseOwner() { m_snapPoint.pSnapOwner = nullptr; }

    //virtual void ChangeSprite(int mtrlNum); // 이거 이미지 or 위젯이 들고 있어도 되는 거 아님??
protected:
    // 아직까지 배열로 활용할진 모르겠으니 최소 형태로 한 뒤 추후 배열로 확대합시다.
    // CardTable같은 상위 위젯에서, 해당 위젯을 여러 개 소유한 형태가 될 수도 있기 때문.

    //std::vector<SnapPoint> m_snapPoint;    // 위젯이 배치할 구간이 있다면
    SnapPoint m_snapPoint;    // 위젯이 배치할 구간이 있다면
};


inline RECT ExpandRect(const RECT& r, float padding) // 보정치를 통한 RECT 범위 확장 (RECT + padding)
{
    // float타입인 padding을 Long타입인 Rect에 맞추기 위해 아래 세 함수중에 보정할 것.
    // 
    // floor(n)  : 내림(−방향). 예) floor( 1.2)= 1, floor(-1.2)=-2
    // ceil(n)   : 올림(+방향). 예) ceil ( 1.2)= 2, ceil (-1.2)=-1
    // lround(n) : 가장 가까운 정수로 반올림. 예) lround(1.4)=1, lround(1.5)=2, lround(-1.5)=-2

    LONG p = (LONG)std::ceil(padding); // 패딩 보정 : ceil // 판정은 넉넉하게..

    RECT out = r;
    out.left -= p;
    out.top -= p;
    out.right += p;
    out.bottom += p;
    return out;
}
