#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "YunoTransform.h"
#include "Mesh.h"

constexpr Float2 g_defWidgetSize{ 50, 50 };

enum class Visibility : uint8_t { Visible, Hidden, Collapsed };

//class UIManager;
class UIFactory;
struct WidgetDesc;

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
    ProgressBar,
    Gauge,
    //Slider,
    Count,
};

enum class WidgetClass : int {
    Widget,
    Image,
    Button,
    Text,
    Slot,
    ProgressBar,
    Gauge,
    CardTable,
    Card,
    CardSlot,
    LetterBox,
    GridLine,

    // 첫 무기 선택 페이즈
    UserImage,
    TitleImage,
    ReadyButton,
    ExitButton,
    WeaponButton,

    // 전투 씬
    BarPanel,
    PieceImage,
    HealthBar,
    HealthGauge,
    StaminaBar,
    StaminaGauge,
};

enum class WidgetLayer : int {
    Default,
    Background,
    HUD,
    Panels,
    Popups,
    Modal,    // 입력 차단
    Tooltip,  
    DebugTop, // 디버그용 최상단 고정 레이어
    Count,
};

struct Float2;
struct Float3;
struct Float4;

constexpr float g_PivotMin = 0.0f;
constexpr float g_PivotMax = 1.0f;

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

constexpr const Float2 PivotFromUIDirection(UIDirection pivot) { // 피벗 전용 할당값
    const int i = (int)pivot;
    assert(0 <= i && i < (int)UIDirection::Count);
    return kPivot[(int)pivot];
}

constexpr bool PivotMinMax(Float2 pivot) { // 피벗 최소 최대치 비교
    return (pivot.x >= g_PivotMin &&
            pivot.y >= g_PivotMin &&
            pivot.x <= g_PivotMax &&
            pivot.y <= g_PivotMax);
}

// Left Top 기준 Pos를 넣으면, 피벗 적용 Pos 반환
inline constexpr XMFLOAT3 MakePivotPosFromLT(const XMFLOAT3& ltPos, const Float2& sizePx, UIDirection pivot)
{
    Float2 pv = PivotFromUIDirection(pivot); // (0~1)
    return XMFLOAT3(
        ltPos.x + sizePx.x * pv.x,
        ltPos.y + sizePx.y * pv.y,
        ltPos.z
    );
}

// Left Top 기준 Pos를 넣으면, 피벗 적용 Pos 반환
inline constexpr XMFLOAT3 MakePivotPosFromLT(const XMFLOAT3& ltPos, const Float2& sizePx, Float2 pivot)
{
    Clamp(pivot);
    return XMFLOAT3(
        ltPos.x + sizePx.x * pivot.x,
        ltPos.y + sizePx.y * pivot.y,
        ltPos.z
    );
}

constexpr Float2 g_DefaultClientXY{ 1920,1080 };

class Widget
{
protected:

    // 정보 데이터

    uint32_t m_id;
    WidgetType m_type;
    WidgetLayer m_layer; // 자식은 부모의 레이어를 따라감

    std::wstring m_name;

    // 트랜스폼 데이터

    XMFLOAT3	m_vPos;     // 스크린상의 위치 (z는 사용 안 함(camera-near 문제))
    XMFLOAT3	m_vRot;     // 스크린상의 위젯 Rot
    XMFLOAT3	m_vScale;   // 스크린상의 위젯 크기 배율 (z는 의미 없음, 사용 안함)
    
    XMFLOAT4X4	m_mWorld; //m_mWorldWithSize; // 렌더/Rect/스냅에 쓰는 월드 (m_size 사용, 자식 결합에 사용)
    XMFLOAT4X4& m_mWorldWithSize = m_mWorld;  // m_mWorld 별명
    XMFLOAT4X4	m_mWorldNoSize;               // 부모가 자식에게 상속하는 월드 (m_size 안씀, 부모만 사용)

    

    XMFLOAT4X4  m_mSize;
    XMFLOAT4X4	m_mScale;
    XMFLOAT4X4  m_mRot;
    XMFLOAT4X4  m_mTrans;
    XMFLOAT4X4  m_mPivot;

    XMFLOAT3	m_vPosBk;
    XMFLOAT3	m_vRotBk;
    XMFLOAT3	m_vScaleBk;
    XMFLOAT3 	m_vDirBk;

    Float2 m_pivot;             // 위젯 피벗 (보정위치)


    // 사이즈 데이터

    Float2 m_size;               // 위젯 자체의 사이즈 (width, height)s

    std::vector<Float2> m_textureSizes;

    Float3 m_finalPos;

    Float3 m_finalScale;           // 최종 위젯 스케일 XY // m_finalScale * m_canvasScale

    // 캔버스 관련 데이터

    Float3 m_canvasSize;        // 캔버스 사이즈 XY (아무런 캔버스도 없을 땐 클라이언트 사이즈 = 클라가 캔버스 역할)
          
    //Float3 m_clientSize;        // 클라이언트 사이즈 XY

    Float3 m_canvasScale;           // 해상도 대응 스케일 (canvasSizeXY/clientSizeXY) - Scale에 적용
    Float3 m_canvasLetterboxOffset; // 레터박스 보정 오프셋 - Pos에 적용
    //Canvas* m_canvas;


    // 기타 데이터

    RECT m_rect;                // 현재 위젯을 RECT로 치환한 값

    int m_zOrder;               // 아직 미사용

    bool m_mirrorX = false;

    Visibility m_visible;       // 보이기 여부 // 아직 미사용

    std::wstring m_inputString; // 텍스트 입력 내용 // 아직 미사용

    // 자주 변하지 않는 UI 최적화를 위한 더티 플래그.
    // 자식이 있을 경우 자식들의 더티 플래그도 조절해야 함.
    bool m_transformDirty = true; // pos,rot,scale 변경 시

    float       m_time;

    RenderItem      m_renderItem;
    MeshHandle      m_defaultMesh;          // 현재 사용 메쉬
    MaterialHandle  m_defaultMaterial;      // 현재 사용 머테리얼
    std::vector<MaterialHandle> m_materials; // 스왑용 현재(0) + 추가 머테리얼

    TextureHandle   m_Albedo;
    TextureHandle   m_Normal;
    TextureHandle   m_Orm;

    // 상수버퍼 업데이트할 데이터들
    Update_Data   m_constant;

    std::unique_ptr<MeshNode> m_MeshNode;

    Widget* m_Parent;
    std::unordered_map<uint32_t, Widget*> m_Childs;

    bool m_isRoot = true; // 캔버스를 제외한 가장 최상위 부모인지. // 나중에 캔버스 위젯이 생기면 캔버스만 예외처리 해야 함.

    bool m_useAspectComp = true; // 업데이트 시 캔버스/클라이언트 스케일링을 사용할건지 아닌지 // 기본값 : 사용

protected:

    IRenderer* m_pRenderer = nullptr;

    ITextureManager* m_pTextures = nullptr;

    IInput* m_pInput = nullptr;

    UIDirection m_anchor; // 아직 안씀

   //UIManager* m_pUIManager = nullptr; // UIManager
    UIFactory& m_uiFactory;


public:
    // 테스트
    Widget() = delete; // 기본 생성 금지
    explicit Widget(UIFactory& uiFactory);
    virtual ~Widget();

    //Create는 오브젝트 매니저만 쓰기
    virtual bool  Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos);
    virtual bool  Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale);
    virtual bool  Start(); // Create 다 끝나고 호출. 
    virtual void  CreateChild() {};

    virtual bool  UpdateTransform(float dTime = 0); // 트랜스폼 업데이트. 자식에서 호출하면 꼬이니 주의
    virtual bool  Update(float dTime = 0);  
    virtual bool  Submit(float dTime = 0);
    bool          LastSubmit(float dTime = 0);      // 이거는 오버라이드 X

    //////////////////////////////////////////////////
    // 자식 체이닝(Private)
    private: 
    bool          UpdateTransformChild(float dTime = 0);     // 자식 체이닝 진입조건
    void          UpdateTransformChild_Internal(float dTime = 0);     // 자식 체이닝 루프

    bool          SubmitChild(float dTime = 0);              // 자식 체이닝 진입조건
    void          SubmitChild_Internal(float dTime = 0);     // 자식 체이닝 루프
    //void          UpdateTransformTree(bool parentDirty);     // 트랜스폼 갱신 더티 체크 (업데이트 로직에서 Tarnsform 갱신 대비)
    //void          UpdateTransformSelf();                     // Tarnsform 더티면 갱신
    //////////////////////////////////////////////////
    public:

    void          UpdateRect();
    void          UpdateRectWorld();

    // 위치 세팅 // 더티 플래그는 아직 미사용
    void          SetSize(Float2 size)          { m_size = size; m_transformDirty = true; }
    void          SetPos(XMFLOAT3 vPos)         { m_vPos = vPos; m_transformDirty = true; }
    void          SetPosBK(XMFLOAT3 vPosBk)     { m_vPosBk = vPosBk; }
    void          SetRot(XMFLOAT3 vRot)         { m_vRot = vRot; m_transformDirty = true; }
    void          SetRotBK(XMFLOAT3 vRotBk)     { m_vRotBk = vRotBk; }
    void          SetScale(XMFLOAT3 vScale)     { m_vScale = vScale; m_transformDirty = true;}
    void          SetScaleBK(XMFLOAT3 vScaleBk) { m_vScaleBk = vScaleBk; }
    void          SetPivot(Float2 pivot)        { assert(PivotMinMax(pivot)); m_pivot = pivot; m_transformDirty = true; }
    void          SetPivot(UIDirection dir)     { m_pivot = PivotFromUIDirection(dir); m_transformDirty = true;}
    virtual bool  IsCursorOverWidget(POINT mouseXY);    // 마우스 커서가 위젯 위에 있는지 체크
    void          SetCanvasSize(Float3 sizeXY)   { m_canvasSize = sizeXY; m_transformDirty = true;}
    void          SetIsRoot(bool isRoot) { m_isRoot = isRoot; }
    void          SetLayer(WidgetLayer layer) { m_layer = layer; }
    void          SetTextureSize(int num, TextureHandle& handle);


    Float2        AddTextureSize(TextureHandle& handle);

    void          MirrorScaleX() { m_vScale.x *= -1; }
    void          MirrorScaleY() { m_vScale.y *= -1; }

    virtual void  Backup();
    void SetBackUpTransform() { m_vPos = m_vPosBk; m_vRot = m_vRotBk; m_vScale = m_vScaleBk; }

    Float2&                      GetSize() { return m_size; }
    XMFLOAT3&                    GetPos() { return m_vPos; }
    XMFLOAT3&                    GetRot() { return m_vRot; }
    XMFLOAT3&                    GetScale() { return m_vScale; }
    uint32_t                     GetID() { return m_id; }
    const std::wstring&          GetName() const { return m_name; }
    
    XMMATRIX                     GetTransMatrix() { return XMLoadFloat4x4(&m_mTrans); }
    XMMATRIX                     GetRotMatrix() { return XMLoadFloat4x4(&m_mRot); }
    XMMATRIX                     GetScaleMatrix() { return XMLoadFloat4x4(&m_mScale); }
    XMMATRIX                     GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }
    XMMATRIX                     GetWorldWithSizeMatrix() { return XMLoadFloat4x4(&m_mWorldWithSize); } // Rect / 히트테스트용, 스냅용 
    XMMATRIX                     GetWorldNoSizeMatrix() { return XMLoadFloat4x4(&m_mWorldNoSize); } // 자식 결합(부모-자식)체인용 중점 : "간격"
    
    const RECT                   GetRect() const { return m_rect; }
    const Float2                 GetPivot() { return m_pivot; }
    bool                         GetIsRoot() { return m_isRoot; }
    WidgetLayer                  GetLayer() { return m_layer; }
    bool                         HasMeshNode() const { return m_MeshNode.get() != nullptr; }
    const Float3&                GetTextureSize(int num) const { assert(num >= 0 && num < m_textureSizes.size()); return m_textureSizes[num]; }
    const std::vector<Float2>&   GetTextureSizes() const { return m_textureSizes; }
   

    //UI 메쉬는 기본적으로 쿼드이므로 재사용 가능성이 높음
    virtual bool CreateMesh();

    virtual bool CreateMaterial(std::wstring path, MaterialDesc* pDesc = nullptr);

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

    virtual bool AddMaterial(const std::wstring& path, MaterialDesc& desc);

    virtual bool AddMaterial(MaterialDesc& desc);

    virtual void SetMesh(std::unique_ptr<MeshNode>&& mesh);

    bool SwapMaterial(int num);

    void Attach(Widget* obj);
    void DettachParent();
    void DettachChild(uint32_t id);
    void ClearChild();
    void Clear();

    Widget* GetParent() { return m_Parent; }
    std::unordered_map<uint32_t, Widget*>& GetChilds() { return m_Childs; }

    virtual WidgetType GetWidgetType() { return WidgetType::Widget; }
    virtual WidgetClass GetWidgetClass() { return WidgetClass::Widget; }
    

    WidgetDesc BuildWidgetDesc();

#ifdef _DEBUG
    virtual void Serialize() {}
#endif

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


// IsIntersectWin32()와 동일 기능
inline bool IsIntersect(const RECT& a, const RECT& b)
{
    // 경계 닿으면 판정 O
    return !(a.right < b.left ||
        a.left > b.right ||
        a.bottom < b.top ||
        a.top > b.bottom);

    // 경계 닿으면 판정 X
    //return !(a.right <= b.left ||
    //    a.left >= b.right ||
    //    a.bottom <= b.top ||
    //    a.top >= b.bottom);
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
