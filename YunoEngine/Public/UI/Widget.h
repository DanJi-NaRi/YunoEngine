#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "YunoTransform.h"
#include "Mesh.h"

enum class Visibility : uint8_t { Visible, Hidden, Collapsed };

class UIManager;

enum class UIDirection : int {
    LeftTop,
    Top,
    RightTop,
    Left,
    Center,
    Right,
    LeftBottom,
    Bottom,
    RightBottom,
    Count,
};

enum class WidgetType : int { // 자신 / 부모 클래스 타입
    Widget,
    Image,
    Button,
    Text,
    Slot,
    //Progress,
    //Slider,
    Count,
};

enum class WidgetClass : int {
    Widget,
    Image,
    Button,
    Text,
    Slot,
    CardTable,
    Card,
    CardSlot,
    LetterBox,
    // 첫 무기 선택 페이즈
    UserImage,
    TitleImage,
    ReadyButton,
    ExitButton,
    WeaponButton,

};

struct Float2;
struct Float3;
struct Float4;

constexpr float pivotMin = 0.0f;
constexpr float pivotMax = 1.0f;

inline constexpr Float2 kPivot[(int)UIDirection::Count] = {
    {0.0f, 0.0f}, // LeftTop
    {0.5f, 0.0f}, // Top
    {1.0f, 0.0f}, // RightTop
    {0.0f, 0.5f}, // Left
    {0.5f, 0.5f}, // Center
    {1.0f, 0.5f}, // Right
    {0.0f, 1.0f}, // LeftBottom
    {0.5f, 1.0f}, // Bottom
    {1.0f, 1.0f}, // RightBottom
};

constexpr Float2 PivotFromUIDirection(UIDirection pivot) { // 피벗 전용 할당값
    const int i = (int)pivot;
    assert(0 <= i && i < (int)UIDirection::Count);
    return kPivot[(int)pivot];
}

struct SnapPoint {
    XMFLOAT2 m_snapPos; // 스냅 위치 : 기본적으로 slot과 1:1이겠지만, 슬롯이 여러 스냅포인트를 가진 경우 달라질 수 있다.
    RECT m_snapRange;   // 스냅 검사 Rect : 위젯이 해당 Rect와 AABB가 통과되면, snapPos로 스냅한다.
    float m_snapPadding; // 범위 보정치 : Rect 감지 범위에 padding만큼 추가 보정을 한다.
    WidgetClass m_snapTargetClass; // 스냅 조건
    // 추가 조건 있으면 추가...
};

class Widget
{
protected:

    // 정보 데이터

    uint32_t m_id;
    WidgetType m_type;

    std::wstring m_name;

    // 트랜스폼 데이터

    XMFLOAT3	m_vPos;     // 스크린상의 위치 (z는 사용 안 함(camera-near 문제))
    XMFLOAT3	m_vRot;     // 스크린상의 위젯 Rot
    XMFLOAT3	m_vScale;   // 스크린상의 위젯 크기 배율 (z는 의미 없음, 사용 안함)
    
    XMFLOAT4X4	m_mWorld;
    XMFLOAT4X4	m_mScale;
    XMFLOAT4X4  m_mRot;
    XMFLOAT4X4  m_mTrans;

    XMFLOAT3	m_vPosBk;
    XMFLOAT3	m_vRotBk;
    XMFLOAT3	m_vScaleBk;
    XMFLOAT3 	m_vDirBk;

    Float2 m_pivot;             // 위젯 피벗 (보정위치)


    // 사이즈 데이터

    //float m_width;              // 위젯 자체의 가로 사이즈
    //float m_height;             // 위젯 자체의 세로 사이즈
    Float3  m_size;               // 위젯 자체의 사이즈

    //float m_spriteSizeX;        // 적용된 Albedo(스프라이트) 원본 사이즈 X
    //float m_spriteSizeY;        // 적용된 Albedo(스프라이트) 원본 사이즈 Y
    Float2  m_spriteSize;

    //float m_sizeX;              // 최종 위젯 사이즈 X // m_width * m_scale.x * m_canvasOffsetX
    //float m_sizeY;              // 최종 위젯 사이즈 Y // m_height * m_scale.y * m_canvasOffsetY
    Float2 m_finalSize;           // 최종 위젯 사이즈 XY // m_height * m_scale.y * m_canvasOffset




    // 캔버스 관련 데이터

    Float2 m_canvasSize;        // 캔버스 사이즈 XY (아무런 캔버스도 없을 땐 클라이언트 사이즈 = 클라가 캔버스 역할)
    //float m_canvasSizeX;        // 캔버스 사이즈 X (아무런 캔버스도 없을 땐 클라이언트 사이즈 = 클라가 캔버스 역할)
    //float m_canvasSizeY;        // 캔버스 사이즈 Y (아무런 캔버스도 없을 땐 클라이언트 사이즈 = 클라가 캔버스 역할)
          
    Float2 m_clientSize;        // 클라이언트 사이즈 XY

    Float2 m_canvasOffset;       // 캔버스 결과 적용 오프셋 (canvasSizeXY/clientSizeXY)

    //Canvas* m_canvas;




    // 기타 데이터

    RECT m_rect;                // 현재 위젯을 RECT로 치환한 값

    int m_zOrder;               // 아직 미사용

    Visibility m_visible;       // 보이기 여부 // 아직 미사용

    std::wstring m_inputString; // 텍스트 입력 내용 // 아직 미사용


    // 자주 변하지 않는 UI 최적화를 위한 더티 플래그.
    // 자식이 있을 경우 자식들의 더티 플래그도 조절해야 함.
    bool m_layoutDirty    = true; // pos,rot,scale 변경 시
    bool m_transformDirty = true; // width,height 변경 시

    float       m_time;

    RenderItem      m_renderItem;
    MeshHandle      m_defaultMesh;
    MaterialHandle  m_defaultMaterial;
    std::vector<MaterialHandle>  m_addMaterial;
    TextureHandle   m_Albedo;
    TextureHandle   m_Normal;
    TextureHandle   m_Orm;

    // 상수버퍼 업데이트할 데이터들
    Update_Data   m_constant;

    std::unique_ptr<MeshNode> m_MeshNode;

    Widget* m_Parent;
    std::unordered_map<uint32_t, Widget*> m_Childs;

protected:
    IRenderer* m_pRenderer = nullptr;

    ITextureManager* m_pTextures = nullptr;

    IInput* m_pInput = nullptr;

    UIDirection m_anchor; // 아직 안씀

    UIManager* m_pUIManager = nullptr; // UIManager
public:

    Widget() = delete; // 기본 생성 금지
    explicit Widget(UIManager* uiManager);
    virtual ~Widget();

    //Create는 오브젝트 매니저만 쓰기
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);

    virtual bool  Update(float dTime = 0);
    virtual bool  Submit(float dTime = 0);
    bool          LastSubmit(float dTime = 0);      // 이거는 오버라이드 X

    void          UpdateRect();

    // 위치 세팅
    void          SetPos(XMFLOAT3 vPos)         { m_vPos = vPos; }
    void          SetPosBK(XMFLOAT3 vPosBk)     { m_vPosBk = vPosBk; }
    void          SetRot(XMFLOAT3 vRot)         { m_vRot = vRot; }
    void          SetRotBK(XMFLOAT3 vRotBk)     { m_vRotBk = vRotBk; }
    void          SetScale(XMFLOAT3 vScale)     { m_vScale = vScale; }
    void          SetScaleBK(XMFLOAT3 vScaleBk) { m_vScaleBk = vScaleBk; }
    void          SetPivot(Float2 pivot)        { m_pivot = pivot; }
    void          SetPivot(UIDirection dir)     { m_pivot = PivotFromUIDirection(dir); }
    virtual bool  IsCursorOverWidget(POINT mouseXY);    // 마우스 커서가 위젯 위에 있는지 체크
    Float2        SetCanvasSizeX(Float2 sizeXY)   { m_canvasSize = sizeXY; }

    virtual void  Backup();


    //UI 메쉬는 기본적으로 쿼드이므로 재사용 가능성이 높음
    virtual bool CreateMesh();

    bool CreateMaterial(std::wstring path, MaterialDesc* pDesc = nullptr);

    template <typename Path, typename... Paths>
    bool CreateMaterials(Path&& path, Paths&&... paths) // wstring_view 타입일 것
    {
        static_assert(
            std::is_convertible<Path, std::wstring_view>::value &&
            (std::is_convertible<Paths, std::wstring_view>::value && ...),
            "CreateMaterials: all arguments must be convertible to std::wstring_view"
            );

        bool hr;
        hr = CreateMaterial(path);
        hr = (AddMaterial(std::wstring_view(paths)) && ...);
        return hr;
    }

    virtual bool CreateMaterial() { return CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };

    virtual bool AddMaterial(std::wstring path);

    virtual void SetMesh(std::unique_ptr<MeshNode>&& mesh);

    uint32_t GetID() { return m_id; }
    const std::wstring& GetName() const { return m_name; }

    XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }
    const RECT GetRect() const { return m_rect; }
    const Float2 GetPivot() { return m_pivot; }

    void Attach(Widget* obj);
    void DettachParent();
    void DettachChild(uint32_t id);
    void ClearChild();
    void Clear();

    Widget* GetParent() { return m_Parent; }
    std::unordered_map<uint32_t, Widget*>& GetChilds() { return m_Childs; }

    virtual WidgetType GetWidgetType() { return WidgetType::Widget; }
    virtual WidgetClass GetWidgetClass() { return WidgetClass::Widget; }

    //bool IsIntersect(const RECT& other)
    //{
    //    return !(this->m_rect.left <= other.left ||
    //        this->m_rect.left >= other.right ||
    //        this->m_rect.bottom <= other.top ||
    //        this->m_rect.top >= other.bottom);
    //}
    
};

extern MeshHandle g_defaultWidgetMesh;

bool SetupDefWidgetMesh(MeshHandle& meshHandle, IRenderer* renderer);
inline MeshHandle GetDefWidgetMesh(MeshHandle* out = nullptr)
{
    if (out) *out = g_defaultWidgetMesh;
    return g_defaultWidgetMesh;
}




/*
// UI 그리기 (일반 2D)
void UIDraw(const LPSPRITE pSpr, VECTOR3 vPos);
void UIDrawAB(const LPSPRITE pSpr, VECTOR3 vPos, FLOAT alpha = 1.0f);

// UI 그리기 (2.5/3D)
void UIDraw25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1));
void UIDrawAB25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1), FLOAT alpha = 1.0f);
void UIDrawFX25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1), FLOAT alpha = 1.0f);
*/
