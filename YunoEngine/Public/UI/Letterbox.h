#pragma once
#include "Image.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class Letterbox : public Image {
public:
    enum class Role : uint8_t
    {
        BarA, // 좌/우일 때 Left, 상/하일 때 Top
        BarB  // 좌/우일 때 Right, 상/하일 때 Bottom
    };

    explicit Letterbox(UIFactory& uiFactory);
    virtual ~Letterbox();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool UpdateTransform(float dTime = 0) override;
    bool UpdateLogic(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    bool UpdateLetterBox(float dTime = 0);
    void SetRole(Role role) { m_role = role; }

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::LetterBox; }
private:

    bool CreateMesh() override;      // 메시 생성 (한 번만)
    virtual bool CreateMaterial() { return Widget::CreateMaterial(L"../Assets/Textures/black.png"); };
    //void SetMesh(std::unique_ptr<MeshNode>&& mesh);

private:
    Role m_role = Role::BarA;
    // 기준 해상도(디자인 해상도)
    XMFLOAT2 m_origin = { 1920.0f, 1080.0f };
};
