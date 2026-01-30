#pragma once
#pragma once
#include "Image.h"
#include "CardData.h"

class UserImage : public Image
{
public:
    UserImage(UIFactory& uiFactory);
    virtual ~UserImage();
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool UpdateLogic(float dTime = 0) override;
    bool UpdateTransform(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::UserImage; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Test/X.png"); };    // 머테리얼 생성 (한 번만)

    PieceType GetPieceType() { return  m_selectPiece; }
    void SetPieceType(PieceType pieceType) { m_selectPiece = pieceType; }

    bool ChangeMaterial(int num);

protected:
    bool m_userJoined = false;
    PieceType m_selectPiece = PieceType::None;
};
