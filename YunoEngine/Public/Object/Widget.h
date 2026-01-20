#pragma once

#include "YunoEngine.h"
#include "RenderTypes.h"
#include "IRenderer.h"
#include "ITextureManager.h"
#include "Mesh.h"

//struct Rect { float l = 0, t = 0, r = 0, b = 0; }; // left, top, right, bottom
//struct Rect { float width, height; };
struct RectPx { float x = 0, y = 0, w = 0, h = 0; };

// 피봇 예정...
enum class Visibility : uint8_t { Visible, Hidden, Collapsed };
enum class WIDGET_TYPE {
    WIDGET_SPRITE,
    WIDGET_BUTTON,
    WIDGET_PROGRESS,
    WIDGET_SLIDER,
};

class Widget
{
protected:

    uint32_t m_id;
    WIDGET_TYPE m_type;

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

    int m_zOrder;

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
    TextureHandle   m_Albedo;
    TextureHandle   m_Normal;
    TextureHandle   m_Orm;

    RectPx m_layoutRectPx;
    RectPx m_clipRectPx;

    // 상수버퍼 업데이트할 데이터들
    Update_Data   m_constant;

    std::unique_ptr<MeshNode> m_MeshNode;

    Widget* m_Parent;
    std::unordered_map<uint32_t, Widget*> m_Childs;

protected:
    IRenderer* m_pRenderer = nullptr;
    ITextureManager* m_pTextures = nullptr;
    IInput* m_pInput = nullptr;

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

    virtual void  Backup();

    //UI 메쉬는 기본적으로 쿼드이므로 재사용 가능성이 높음
    virtual bool CreateMesh();
    virtual bool CreateMaterial() { return false; };

    virtual void SetMesh(std::unique_ptr<MeshNode>&& mesh);

    uint32_t GetID() { return m_id; }
    const std::wstring& GetName() const { return m_name; }

    XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&m_mWorld); }

    //void Attach(Widget* obj);
    //void DettachParent();
    //void DettachChild(uint32_t id);
    //void ClearChild();
};

extern MeshHandle g_defaultWidgetMesh;

bool SetupDefWidgetMesh(MeshHandle& meshHandle, IRenderer* renderer);
inline MeshHandle GetDefWidgetMesh(MeshHandle* out = nullptr)
{
    if (out) *out = g_defaultWidgetMesh;
    return g_defaultWidgetMesh;
}

//////////////////////////////////////////////////////////////////////////////
//
// 버튼 상태 : 대기/커서입력/눌림 (standby/CursorOn/Push)
//
//enum BSTATE { BS_STANDBY, BS_CURSORON, BS_PUSH, BS_MAX };		//UI버튼 상태: "대기", "커서올라감", "눌림" 
//BOOL g_BtnState[3] = { BS_STANDBY, };							//UI버튼별 상태 저장.

/*
#define MS_STANDBY   0x0000		//대기중.
#define MS_LBTNDOWN  0x0100		//버튼 눌림.
#define MS_LBTNUP	 0x8100		//버튼 눌린후 떨어짐.
#define MS_RBTNDOWN  0x0001		//버튼 눌림.
#define MS_RBTNUP	 0x0081		//버튼 눌린후 떨어짐.
*/

/*
// UI 그리기 (일반 2D)
void UIDraw(const LPSPRITE pSpr, VECTOR3 vPos);
void UIDrawAB(const LPSPRITE pSpr, VECTOR3 vPos, FLOAT alpha = 1.0f);

// UI 그리기 (2.5/3D)
void UIDraw25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1));
void UIDrawAB25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1), FLOAT alpha = 1.0f);
void UIDrawFX25(const LPSPRITE pSpr, VECTOR3 vPos, VECTOR3 vRot = VECTOR3(0, 0, 0), VECTOR3 vScale = VECTOR3(1, 1, 1), FLOAT alpha = 1.0f);
*/
