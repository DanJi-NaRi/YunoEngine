#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmArea.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"
#include "Minimap.h"
#include "CardSelectionPanel.h"
#include "WeaponNameImage.h"


#include "BattlePackets.h"

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
    m_confirmReady = false;
    m_dirChoice = false;
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

    m_cardConfirmButton->SetEventLMB([this]() {
        if (!BuildCardQueueFromSlots())
            return;
        m_gameManager.SubmitTurn(m_gameManager.GetCardQueue());
        this->m_cardConfirmButton->SetIsClicked(true);
        m_pSelectionPanel->ViewCardPage(0, 0);
        m_pSelectionPanel->GetWeaponNameImage()->ChangeWeaponImage(m_player.weapons[0].weaponId);/* m_curSlot = 0*/;
        m_gameManager.SetSceneState(CurrentSceneState::BattleStandBy);
        });


    m_cardCancelButton = m_uiFactory.CreateChild<CardCancelButton>(m_name + L"_CardCancelButton", Float2(367, 69), XMFLOAT3(700, -100, 0), UIDirection::LeftTop, this);
    m_cardCancelButton->SetEventLMB([this]() 
        { 
            if (m_gameManager.GetSceneState() == CurrentSceneState::BattleStandBy) return;
            this->ClearSlot(); 
            
            assert(m_pMinimap);
            if (m_pMinimap)
                m_pMinimap->DefaultSetAllTile();
            // 만들었는데 생각해보니까 우리 카드 제출 후 취소를 못하네?
            //if (m_gameManager.GetSceneState() == CurrentSceneState::StandBy) 
            //{
            //    m_gameManager.RequestScenePop();        // StandBy씬이면 Pop하기
            //    m_gameManager.SetState(CurrentSceneState::SubmitCard);
            //}
        }
    );

}

bool CardConfirmPanel::Update(float dTime)
{
    PhasePanel::Update(dTime);

    const CurrentSceneState currentState = m_gameManager.GetSceneState();

    if (currentState == CurrentSceneState::AutoBattle && m_cardConfirmButton->GetIsClicked())
    {
        ClearSlot();
    }

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
    const int size = static_cast<int>(m_setCardSlots.size());

    if (m_openSlot < 0 || m_openSlot >= size)
        return;

    CardConfirmArea* currentSlot = m_setCardSlots[m_openSlot];
    if (!currentSlot)
        return;

    if (!currentSlot->GetCard())
        return; // 현재 슬롯이 비어있으면 넘김

    if (!m_dirChoice)
    {
        currentSlot->SetIsEnabled(false);
        m_dirChoice = true;
        m_pMinimap->StartDirChoice(currentSlot);
        return;
    }

    if (currentSlot->GetDirection() == Direction::None)
        return;

    SubmitCurrentSelection();
}


void CardConfirmPanel::ClearSlot() {
    if (m_hasSimulatedStamina && m_cardConfirmButton && !m_cardConfirmButton->GetIsClicked())
    {
        for (int i = 0; i < static_cast<int>(m_originalStamina.size()); ++i)
        {
            m_player.weapons[i].stamina = m_originalStamina[i];
        }
    }
    m_openSlot = 0;
    m_dirChoice = false;
    m_hasSimulatedStamina = false;
    m_gameManager.ClearCardQueue();
    m_cardConfirmButton->SetIsClicked(false);

    if (m_pSelectionPanel)
        m_pSelectionPanel->ClearSelectedCard();

    if (!m_setCardSlots.empty())
    {
        // 현재 슬롯이 비어있지 않으면
        m_setCardSlots[0]->SetIsEnabled(true);
        m_setCardSlots[0]->CleanSetup();
    }

    for (size_t i = 1; i < m_setCardSlots.size(); ++i) {
        m_setCardSlots[i]->SetIsEnabled(false);
        m_setCardSlots[i]->CleanSetup();
    }
    m_confirmReady = false; // 컨펌 준비 취소
}


bool CardConfirmPanel::IsCardAlreadyQueuedInSlots(uint32_t runtimeID, const CardConfirmArea* ignoreSlot) const
{
    for (const CardConfirmArea* queuedSlot : m_setCardSlots)
    {
        if (!queuedSlot || queuedSlot == ignoreSlot)
            continue;

        const int queuedRuntimeID = queuedSlot->GetRuntimeCardID();
        if (queuedRuntimeID <= 0)
            continue;

        if (static_cast<uint32_t>(queuedRuntimeID) == runtimeID)
            return true;
    }

    return false;
}
bool CardConfirmPanel::HasEnoughStaminaForCard(int unitSlot, uint32_t runtimeID) const
{
    if (unitSlot < 0 || unitSlot >= static_cast<int>(m_simulatedStamina.size()))
        return false;

    const CardData cardData = m_gameManager.GetCardData(runtimeID);
    const int currentStamina = m_simulatedStamina[unitSlot];

    return currentStamina >= cardData.m_cost;
}

void CardConfirmPanel::SyncSimulatedStaminaFromPlayer()
{
    for (int i = 0; i < static_cast<int>(m_simulatedStamina.size()); ++i)
    {
        m_originalStamina[i] = m_player.weapons[i].stamina;
        m_simulatedStamina[i] = m_player.weapons[i].stamina;
    }
    m_hasSimulatedStamina = true;
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

    Card* selectedCard = slot->GetCard();


    if (!selectedCard) {
        selectedCard = m_pSelectionPanel->GetSelectedCard();
        if (!selectedCard)
            return;

        if (selectedCard->GetCardID() == 0)
            return;

        slot->SetCard(selectedCard);
    }


    const int selectedRuntimeIDRaw = slot->GetRuntimeCardID();
    if (selectedRuntimeIDRaw <= 0)
        return;

    const auto selectedRuntimeID = static_cast<uint32_t>(selectedRuntimeIDRaw); 

    if (selectedRuntimeID == 0)
        return;

    if (IsCardAlreadyQueuedInSlots(selectedRuntimeID, slot))
        return;


    if (!m_hasSimulatedStamina)
    {
        SyncSimulatedStaminaFromPlayer();
    }


    const int selectedSlot = (slot->GetCardSlotID() >= 0) ? slot->GetCardSlotID() : m_pSelectionPanel->GetCurrentSlot();


    if (!HasEnoughStaminaForCard(selectedSlot, selectedRuntimeID))
        return;

    const Direction dir = slot->GetDirection();
    if (dir == Direction::None)
        return;



    const CardData cardData = m_gameManager.GetCardData(selectedRuntimeID);
    m_simulatedStamina[selectedSlot] -= cardData.m_cost;
    m_player.weapons[selectedSlot].stamina = m_simulatedStamina[selectedSlot];



    slot->ChangeTexture(selectedCard->GetTexturePath());
    slot->SetDirection(dir);
    slot->SetIsEnabled(false);

    ++m_openSlot;
    m_dirChoice = false;
    m_pSelectionPanel->ClearSelectedCard();

    if (m_openSlot < static_cast<int>(m_setCardSlots.size()))
    {
        m_setCardSlots[m_openSlot]->SetIsEnabled(true);
        m_confirmReady = false;
    }
    else
    {
        m_confirmReady = true;
    }
}

bool CardConfirmPanel::BuildCardQueueFromSlots()
{
    m_gameManager.ClearCardQueue();

    for (CardConfirmArea* slot : m_setCardSlots)
    {
        if (!slot)
        {
            m_gameManager.ClearCardQueue();
            return false;
        }

        const int runtimeID = slot->GetRuntimeCardID();
        if (runtimeID <= 0)
        {
            m_gameManager.ClearCardQueue();
            return false;
        }

        const Direction dir = slot->GetDirection();
        if (dir == Direction::None)
        {
            m_gameManager.ClearCardQueue();
            return false;
        }

        CardPlayCommand cmd;
        cmd.runtimeID = static_cast<uint32_t>(runtimeID);
        cmd.dir = dir;

        if (!m_gameManager.PushCardCommand(cmd))
        {
            m_gameManager.ClearCardQueue();
            return false;
        }
    }

    return m_gameManager.IsCardQueueFull();
}

void CardConfirmPanel::UpdatePanel(const BattleResult& battleResult)
{
}

void CardConfirmPanel::UpdatePanel(const ObstacleResult& obstacleResult)
{
}
