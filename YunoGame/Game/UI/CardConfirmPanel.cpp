#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmArea.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"
#include "Minimap.h"

#include "BattlePackets.h"

#include "IInput.h"
#include "UIFactory.h"



CardConfirmPanel::CardConfirmPanel(UIFactory& uiFactory) : PhasePanel(uiFactory)
{
    Clear();
}

CardConfirmPanel::~CardConfirmPanel()
{
    Clear();
}

void CardConfirmPanel::Clear()
{
    m_openSlot = 0;
    m_pMinimap = nullptr;
}


bool CardConfirmPanel::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    PhasePanel::Create(name, id, sizePx, vPos, rotZ, vScale);

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

bool CardConfirmPanel::Start()
{
    PhasePanel::Start();

    return true;
}

void CardConfirmPanel::CreateChild() {

    // 고정 하위 위젯 생성


    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S0", Float2(178, 264), XMFLOAT3(50, -400, 0),  UIDirection::LeftTop, this));

    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S1", Float2(178, 264), XMFLOAT3(350, -400, 0), UIDirection::LeftTop, this));
    m_setCardSlots.back()->SetIsEnabled(false);

    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S2", Float2(178, 264), XMFLOAT3(650, -400, 0), UIDirection::LeftTop, this));
    m_setCardSlots.back()->SetIsEnabled(false);

    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S3", Float2(178, 264), XMFLOAT3(950, -400, 0), UIDirection::LeftTop, this));
    m_setCardSlots.back()->SetIsEnabled(false);

    m_cardConfirmButton = m_uiFactory.CreateChild<CardConfirmButton>(m_name + L"_CardConfirmButton", Float2(367, 69), XMFLOAT3(0, -100, 0), UIDirection::LeftTop, this);
    
    m_cardCancelButton = m_uiFactory.CreateChild<CardCancelButton>(m_name + L"_CardCancelButton", Float2(367, 69), XMFLOAT3(700, -100, 0), UIDirection::LeftTop, this);
    m_cardCancelButton->SetEventLMB([this]() { this->ClearSlot(); });

}

bool CardConfirmPanel::Update(float dTime)
{
    PhasePanel::Update(dTime);
    UpdateCardSlot();

    return true;
}

bool CardConfirmPanel::Submit(float dTime)
{
    PhasePanel::Submit(dTime);
    return false;
}

void CardConfirmPanel::UpdateCardSlot()
{
    assert(m_pMinimap); // 미니맵 할당되어야 함
    const int size = m_setCardSlots.size();

    if (m_openSlot >= size) return;

    if (!m_setCardSlots[m_openSlot]->GetCard()) return; // 현재 슬롯이 비어있으면 넘김

    if (!m_dirChoice)
    {
        // 현재 슬롯이 비어있지 않으면
        m_setCardSlots[m_openSlot]->SetIsEnabled(false);
        m_dirChoice = true;
        // 선택창 띄우기
        m_pMinimap->StartDirChoice(m_setCardSlots[m_openSlot]); // 미니맵 클릭 활성화
    }
    else {
        if (!m_pMinimap->GetButtonLock()) { // 선택이 완료되면 ButtonLock이 풀릴 것임
            //m_setCardSlots[m_openSlot]->SetDirection(Direction::);
        }
    }
}

void CardConfirmPanel::ClearSlot() {
    m_openSlot = 0;
    m_setCardSlots[0]->SetIsEnabled(true);
    m_setCardSlots[0]->CleanSetup();
    for (int i = 1; i < m_setCardSlots.size(); ++i) {
        m_setCardSlots[i]->SetIsEnabled(false);
        m_setCardSlots[i]->CleanSetup();
    }
}

void CardConfirmPanel::UpdatePanel(const BattleResult& battleResult)
{
}

void CardConfirmPanel::UpdatePanel(const ObstacleResult& obstacleResult)
{
}
