#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "Mesh.h"

enum class Visibility : uint8_t { Visible, Hidden, Collapsed };

enum class Anchor : int {
    LeftTop,
    Top,
    RightTop,
    Left,
    Center,
    Right,
    LeftBottom,
    Bottom,
    RightBottom,
    Max,
};

enum class WidgetType : int { // 자신 / 부모 클래스 타입
    Widget,
    Image,
    Button,
    Text,
    //Progress,
    //Slider,
    Max,
};

enum class WidgetClass : int {
    Widget,
    Image,
    Button,
    Text,
    CardTable,
    Card,
    CardSlot,
};

struct SnapPoint {
    XMFLOAT2 m_snapPoint;
    RECT m_snapRange;
    WidgetClass m_snapTargetClass; // 스냅 조건
};

class Widget
{
protected:

    uint32_t m_id;
    WidgetType m_type;

    std::wstring m_name;

    XMFLOAT3	m_vPos;     // 스크린상의 위치 (z는 사용 안 함(camera-near 문제))
    XMFLOAT3	m_vRot;     // 스크린상의 위젯 Rot
    XMFLOAT3	m_vScale;   // 스크린상의 위젯 크기 배율 (z는 의미 없음, 사용 안함)
    
    XMFLOAT4X4	m_mWorld;
    XMFLOAT4X4	m_mScale, m_mRot, m_mTrans;

    XMFLOAT3	m_vPosBk;
    XMFLOAT3	m_vRotBk;
    XMFLOAT3	m_vScaleBk;
    XMFLOAT3 	m_vDirBk;

    float m_width;
    float m_height;

    float m_spriteSizeX;
    float m_spriteSizeY;

    int m_zOrder; // 아직 미사용

    XMFLOAT2 m_pivot = { 0,0 }; // 아직 미사용

    Visibility m_visible; // 보이기 여부

    std::wstring m_inputString; // 텍스트 입력 내용

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

    RECT m_rect;

    // 상수버퍼 업데이트할 데이터들
    Update_Data   m_constant;

    std::unique_ptr<MeshNode> m_MeshNode;

    Widget* m_Parent;
    std::unordered_map<uint32_t, Widget*> m_Childs;

    std::vector<SnapPoint> m_snapPoint;    // 위젯이 배치할 구간이 있다면

protected:
    IRenderer* m_pRenderer = nullptr;

    ITextureManager* m_pTextures = nullptr;

    IInput* m_pInput = nullptr;

    Anchor m_anchor; // 아직 안씀

public:
    explicit Widget();

    virtual ~Widget();
    //Create는 오브젝트 매니저만 쓰기
    virtual bool  Create(XMFLOAT3 vPos);//일단 호환용으로 냅두고 나중에 무조건 이름 필요한걸로 바꾸는게 나을듯
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    virtual bool  Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);

    virtual bool  Update(float dTime = 0);
    virtual bool  Submit(float dTime = 0);
    bool          LastSubmit(float dTime = 0);      // 이거는 오버라이드 xx

    virtual bool  IsCursorOverWidget(POINT mouseXY);    // 마우스 커서가 위젯 위에 있는지 체크

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
    RECT GetRect() const { return m_rect; }
    XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }

    void Attach(Widget* obj);
    void DettachParent();
    void DettachChild(uint32_t id);
    void ClearChild();

    virtual WidgetType GetWidgetType() { return WidgetType::Widget; }
    virtual WidgetClass GetWidgetClass() { return WidgetClass::Widget; }
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
