#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmArea.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"
#include "CardSelectionPanel.h"

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
    m_cardConfirmButton->SetEventLMB([this]() {
        if (m_gameManager.IsCardQueueEmpty()) return;
        m_gameManager.SubmitTurn(m_gameManager.GetCardQueue());
        });


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
    if (m_gameManager.IsCardQueueFull())
        return;

    SubmitCurrentSelection();
}


void CardConfirmPanel::ClearSlot() {
    m_openSlot = 0;
    m_dirChoice = false;
    m_gameManager.ClearCardQueue();

    if (m_pSelectionPanel)
        m_pSelectionPanel->ClearSelectedCard();

    if (!m_setCardSlots.empty())
    {
        // 현재 슬롯이 비어있지 않으면
        m_setCardSlots[0]->SetIsEnabled(true);
        m_setCardSlots[0]->CleanSetup();
        // 선택창 띄우기

    }


    for (size_t i = 1; i < m_setCardSlots.size(); ++i) {
        m_setCardSlots[i]->SetIsEnabled(false);
        m_setCardSlots[i]->CleanSetup();
    }
}

bool CardConfirmPanel::HandleDirectionInput(Direction& outDir) const
{
    outDir = Direction::None;

    if (m_pInput->IsKeyPressed(VK_UP)) outDir = Direction::Up;
    if (m_pInput->IsKeyPressed(VK_DOWN)) outDir = Direction::Down;
    if (m_pInput->IsKeyPressed(VK_LEFT)) outDir = Direction::Left;
    if (m_pInput->IsKeyPressed(VK_RIGHT)) outDir = Direction::Right;

    return outDir != Direction::None;
}

void CardConfirmPanel::SubmitCurrentSelection()
{
    if (!m_pSelectionPanel || m_setCardSlots.empty())
        return;

    if (m_openSlot < 0 || m_openSlot >= static_cast<int>(m_setCardSlots.size()))
        return;

    CardConfirmArea* slot = m_setCardSlots[m_openSlot];
    if (!slot)
        return;

    Card* selectedCard = m_pSelectionPanel->GetSelectedCard();
    if (!selectedCard)
        return;

    if (selectedCard->GetCardID() == 0)
        return;

    Direction dir = Direction::None;
    if (!HandleDirectionInput(dir))
        return;

    CardPlayCommand cmd;
    cmd.runtimeID = static_cast<uint32_t>(selectedCard->GetCardID());
    cmd.dir = dir;

    if (!m_gameManager.PushCardCommand(cmd))
        return;

    slot->ChangeTexture(selectedCard->GetTexturePath());
    slot->SetDirection(dir);
    slot->SetIsEnabled(false);

    ++m_openSlot;
    m_pSelectionPanel->ClearSelectedCard();

    if (m_openSlot < static_cast<int>(m_setCardSlots.size()))
    {
        m_setCardSlots[m_openSlot]->SetIsEnabled(true);
    }
}

void CardConfirmPanel::UpdatePanel(const BattleResult& battleResult)
{
}

void CardConfirmPanel::UpdatePanel(const ObstacleResult& obstacleResult)
{
}
