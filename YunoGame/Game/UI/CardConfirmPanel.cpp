#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmArea.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"
#include "Minimap.h"
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
        if (!m_gameManager.IsCardQueueFull()) return;
        m_gameManager.SubmitTurn(m_gameManager.GetCardQueue());
        this->m_cardConfirmButton->SetIsClicked(true);
        m_gameManager.SetSceneState(CurrentSceneState::BattleStandBy);
        });


    m_cardCancelButton = m_uiFactory.CreateChild<CardCancelButton>(m_name + L"_CardCancelButton", Float2(367, 69), XMFLOAT3(700, -100, 0), UIDirection::LeftTop, this);
    m_cardCancelButton->SetEventLMB([this]() 
        { 
            if (m_gameManager.GetSceneState() == CurrentSceneState::BattleStandBy) return;
            this->ClearSlot(); 
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
        if (m_setCardSlots[m_openSlot]->GetDirection() != Direction::None) { // 선택이 완료되면 Dir이 바뀔것임

            //디버깅 코드
            switch (m_setCardSlots[m_openSlot]->GetDirection())
            {
            case Direction::Left:  std::cout << "Left selected" << std::endl;  break;
            case Direction::Right: std::cout << "Right selected" << std::endl; break;
            case Direction::Up:    std::cout << "Up selected" << std::endl;    break;
            case Direction::Down:  std::cout << "Down selected" << std::endl;  break;
            }


            // 방향 선택 되면
            const int next = m_openSlot + 1;
            if (next >= size) { // 마지막까지 도달
                m_openSlot = size;
                //m_confirmReady = true;
                return;
            }
            m_openSlot = next;
            m_setCardSlots[m_openSlot]->SetIsEnabled(true);
            m_dirChoice = false; // dirChoice 상태 초기화
        }
    }



    if (m_gameManager.IsCardQueueFull())
        return;

    SubmitCurrentSelection();
}


void CardConfirmPanel::ClearSlot() {
    m_openSlot = 0;
    m_dirChoice = false;
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
    //m_confirmReady = false; // 컨펌 준비 취소
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
