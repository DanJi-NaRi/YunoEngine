#include "pch.h"
#include "WeaponNameImage.h"

#include "IInput.h"
#include "UIFactory.h"

#include "CardData.h"

WeaponNameImage::WeaponNameImage(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

WeaponNameImage::~WeaponNameImage()
{
    Clear();
}

void WeaponNameImage::Clear()
{
}

bool WeaponNameImage::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용
    if (m_defaultMesh == 0)return false;
    if (!CreateMaterial())
        return false;

    m_MeshNode = std::make_unique<MeshNode>();

    auto mesh = std::make_unique<Mesh>();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    m_MeshNode->m_Meshs.push_back(std::move(mesh));

    {
        m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
        m_constant.roughRatio = 1.0f;
        m_constant.metalRatio = 1.0f;
        m_constant.shadowBias = 0.005f;
    }

    m_anchor = UIDirection::LeftTop;

    this->SetLayer(WidgetLayer::Panels);

    Backup();

    return true;
}

bool WeaponNameImage::Start()
{
    Image::Start();

    return true;
}

void WeaponNameImage::CreateChild() {

}

bool WeaponNameImage::Update(float dTime)
{
    Image::Update(dTime);
    return true;
}

bool WeaponNameImage::Submit(float dTime)
{
    Image::Submit(dTime);
    return true;
}

void WeaponNameImage::ChangeWeaponImage(int id)
{
    switch (id + 1) {
    case (int)PieceType::Blaster:   ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_blaster.png"); break;
    case (int)PieceType::Chakram:   ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_chakram.png"); break;
    case (int)PieceType::Breacher:  ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_breacher.png"); break;
    case (int)PieceType::Scythe:    ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_scythe.png"); break;
    case (int)PieceType::Impactor:  ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_impactor.png"); break;
    case (int)PieceType::Cleaver:   ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/Inventory_cleaver.png"); break;
    default: ChangeTexture(L"./Assets/UI/PLAY/PhaseScene/Inventory_test_inventory_non.png"); break;
    }
}

void WeaponNameImage::ChangeWeaponImage(PieceType type)
{
    ChangeWeaponImage((int)type);
}

