#include "pch.h"

#include "GameManager.h"

#include "YunoEngine.h"
#include "ISceneManager.h"

#include "Title.h"
#include "TitleScene.h"
#include "WeaponSelectScene.h"
#include "PlayScene.h"
#include "PhaseScene.h"
#include "StandByScene.h"
#include "CountdownScene.h"
#include "OptionScene.h"
#include "GuideScene.h"

#include "YunoClientNetwork.h"

#include "utilityClass.h"

//패킷
#include "PacketBuilder.h"
#include "C2S_BattlePackets.h"
#include "C2S_ReadySet.h"
#include "C2S_CardPackets.h"
#include "C2S_MatchEnter.h"
#include "C2S_MatchLeave.h"
#include "C2S_Emote.h"

#include "Minimap.h" // 미니맵
#include "CardConfirmPanel.h"   // 카드 컨펌 패널
#include "CardSelectionPanel.h" // 카드 선택 패널

GameManager* GameManager::s_instance = nullptr;


bool GameManager::IsEmptyWeaponData()
{
    return m_weapons.empty();
}

void GameManager::SetWeaponData(int _pId, int _slotId, int _weaponId, int _hp, int _stamina, int _currentTile)
{
    Wdata data;
    data.pId = _pId;
    data.slotId = _slotId;
    data.weaponId = _weaponId;
    data.hp = _hp;
    data.stamina = _stamina;
    data.currentTile = _currentTile;

    m_weapons.push_back(data);
}

void GameManager::SetUIWeaponData(const std::array<Wdata, 4>& wdatas)
{

    for (int i = 0; i < 3; i+=2) 
    {
        if ((i / 2)+1 == m_PID)     // 내가 1P임
        {
            m_myUIWeapons[0] = wdatas[i];
            m_myUIWeapons[1] = wdatas[i+1];
        }
        else                        // 내가 2P임 
        {
            m_enemyUIWeapons[0] = wdatas[i];
            m_enemyUIWeapons[1] = wdatas[i + 1];
        }
        std::cout << std::endl;
    }
}


// 배틀 전용 함수들
void GameManager::PushBattlePacket(const BattleResult _BattleResult)
{
    m_turnPkts.push(_BattleResult);
    std::cout << "Battle Packet is inserted into Queue.\n";
}

BattleResult GameManager::PopBattlePacket()
{
    BattleResult pckt = m_turnPkts.front();
    m_turnPkts.pop();
    std::cout << "Battle Packet is popped out of Queue.\n";
    return pckt;
}

bool GameManager::IsEmptyBattlePacket()
{
    return m_turnPkts.empty();
}


// 장애물 전용 함수들
void GameManager::PushObstaclePacket(const ObstacleResult& obstacleResult)
{
    m_obstaclePkts.push(obstacleResult);
    std::cout << "Obstacle Packet is inserted into Queue.\n";
}

ObstacleResult GameManager::PopObstaclePacket()
{
    ObstacleResult pkt = m_obstaclePkts.front();
    m_obstaclePkts.pop();
    std::cout << "Obstacle Packet is popped out of Queue.\n";
    return pkt;
}

bool GameManager::IsEmptyObstaclePacket()
{
    return m_obstaclePkts.empty();
}

void GameManager::SetConfirmPanel(CardConfirmPanel* pConfirmPanel)
{
    m_pConfirmPanel = pConfirmPanel;
}

void GameManager::SetSelectionPanel(CardSelectionPanel* pSelectionPanel)
{
    m_pSelectionPanel = pSelectionPanel;
}

void GameManager::SetMinimap(Minimap* pMinimap)
{
    m_pMinimap = pMinimap;
}


void GameManager::SetUpPanels()
{
    //assert(m_pMinimap);
    //if (m_pMinimap) m_pMinimap->SetupPanel();
}

void GameManager::UpdatePanels(const BattleResult& battleResult)
{
    // 전투 시 실시간 갱신
    assert(m_pMinimap);
    if (m_pMinimap) m_pMinimap->UpdatePanel(battleResult);

    assert(m_pSelectionPanel);
    if (m_pSelectionPanel) m_pSelectionPanel->UpdatePanel(battleResult);

    assert(m_pConfirmPanel);
    if (m_pConfirmPanel) m_pConfirmPanel->UpdatePanel(battleResult);
}

void GameManager::UpdatePanels(const ObstacleResult& obstacleResult)
{
    // 전투 종료 후
    assert(m_pMinimap);
    if (m_pMinimap) m_pMinimap->UpdatePanel(obstacleResult);

    assert(m_pSelectionPanel);
    if (m_pSelectionPanel) m_pSelectionPanel->UpdatePanel(obstacleResult);

    assert(m_pConfirmPanel);
    if (m_pConfirmPanel) m_pConfirmPanel->UpdatePanel(obstacleResult);
}



void GameManager::Initialize(GameManager* inst)
{
    assert(inst);
    assert(!s_instance);
    s_instance = inst;

}

void GameManager::Init()
{
    m_cardBasicMng.LoadFromCSV("../Assets/CardData/Carddata.csv");
    m_cardRangeMng.LoadFromCSV("../Assets/CardData/CardRange.csv");
}

void GameManager::Shutdown()
{
    assert(s_instance);
    s_instance = nullptr;
}

GameManager& GameManager::Get()
{
    assert(s_instance);
    return *s_instance;
}

void GameManager::SetSceneState(CurrentSceneState state)
{
    if (m_state == state)
        return;
    ISceneManager* sm = YunoEngine::GetSceneManager();
    if (!sm) return;

    switch (state) 
    {
    case CurrentSceneState::Title:
    {
        m_state = CurrentSceneState::Title;
        m_matchPlayerCount = 0;
        ResetMyPicks();
        SceneTransitionOptions opt{};
        opt.immediate = true;
        sm->RequestReplaceRoot(std::make_unique<Title>(), opt);
        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::C2S_MatchLeave,
            [&](yuno::net::ByteWriter& w)
            {
                yuno::net::packets::C2S_MatchLeave req{};
                req.Serialize(w);
            });

        GameManager::Get().SendPacket(std::move(bytes));

        break;
    }
    case CurrentSceneState::RequstEnter:
    {
        m_state = CurrentSceneState::RequstEnter;
        IInput* input = YunoEngine::GetInput();
        ISceneManager* sm = YunoEngine::GetSceneManager();
        
        uint32_t UID = ReadUserIdFromEnv();
        using namespace yuno::net;
        yuno::net::packets::C2S_MatchEnter pkt{};
        pkt.userId = UID;
        
        if (pkt.userId == 0)
        {
            std::cout << "[GameApp] MatchEnter aborted: invalid userId\n";
            m_state = CurrentSceneState::Title;
            return; // 또는 UI 메시지 띄우고 종료
        }
        std::cout << "Env Id : " << pkt.userId << std::endl;
        
        auto bytes = PacketBuilder::Build(
            PacketType::C2S_MatchEnter,
            [&](ByteWriter& w)
            {
                pkt.Serialize(w);
            });
        
        GameManager::Get().SendPacket(std::move(bytes));

        
        break;
    }
    case CurrentSceneState::Option:
    {

        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = true;
        sp.blockInputBelow = true;

        sm->RequestPush(std::make_unique<OptionScene>(), sp);
        break;
    }
    case CurrentSceneState::Guide:
    {
        m_state = CurrentSceneState::Guide;

        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = true;
        sp.blockInputBelow = true;

        sm->RequestPush(std::make_unique<GuideScene>(), sp);
        break;
    }
    case CurrentSceneState::GameStart:
    {
        m_state = CurrentSceneState::GameStart;
        ResetMyPicks();
        SceneTransitionOptions opt{};
        opt.immediate = false;
        sm->RequestReplaceRoot(std::make_unique<WeaponSelectScene>(), opt);

        // 씬에 관련된 데이터들을 같이 넘길거야
        break;
    }
    case CurrentSceneState::StandBy:
    {
        if (m_state == CurrentSceneState::StandBy) return;
        m_state = CurrentSceneState::StandBy;
        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = false;

        sm->RequestPush(std::make_unique<StandByScene>(), sp);

        //SceneTransitionOptions opt{};
        //opt.immediate = false;
        //sm->RequestReplaceRoot(std::make_unique<StandByScene>(), opt);

        break;
    }
    case CurrentSceneState::CountDown:
    {
        m_state = CurrentSceneState::CountDown;
        std::cout << "3...2...1...Battle Start!!!!!" << std::endl;  
        //SceneTransitionOptions opt{};
        //opt.immediate = false; // 보통 false가 자연스러움
        //sm->RequestReplaceRoot(std::make_unique<CountdownScene>(), opt);


        SceneTransitionOptions opt;
        opt.immediate = true;

        sm->RequestPop(opt);

        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = false;
        sm->RequestPush(std::make_unique<CountdownScene>(), sp);

        break;
    }
    case CurrentSceneState::RoundStart:
    {
        // 이미 라운드 씬이면 무시
        //if (m_state == CurrentSceneState::RoundStart) return;
        m_state = CurrentSceneState::RoundStart;
        SceneTransitionOptions opt{};
        opt.immediate = true;

        sm->RequestReplaceRoot(std::make_unique<PlayScene>(), opt);

        SetSceneState(CurrentSceneState::SubmitCard);
        break;
    }
    case CurrentSceneState::SubmitCard:
    {
        m_state = CurrentSceneState::SubmitCard;
        ScenePolicy sp;
        sp.blockRenderBelow = false;
        sp.blockUpdateBelow = false;

        sm->RequestPush(std::make_unique<PhaseScene>(), sp);

        break;
    }
    case CurrentSceneState::AutoBattle:
    {
        m_state = CurrentSceneState::AutoBattle;

        break;
    }
    case CurrentSceneState::RoundEnd:
    {
        break;
    }
    case CurrentSceneState::GameEnd:
    {
        break;
    }
    default:
        assert(false,"No state Scene");
    }
}

void GameManager::InitHands(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    for (int i = 0; i < 2; ++i)
    {
        m_myHands[i].cards.clear();
        m_enemyHands[i].cards.clear();
    }

    m_CardRuntimeIDs.clear();

    AddCards(cards);
}

void GameManager::AddCards(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    for (const auto& card : cards)
    {
        ClientCardInfo info{card.runtimeID, card.dataID };


        bool isMine = (card.PID == m_PID); // PID = 1,2
        int unitIdx = card.slotID - 1;
        
        if(unitIdx < 0 || unitIdx >= 2)
            continue;

        if (isMine)
            m_myHands[unitIdx].cards.push_back(info);
        else
            m_enemyHands[unitIdx].cards.push_back(info);

        m_CardRuntimeIDs[card.runtimeID] = card.dataID;
    }
}

void GameManager::RemoveCard(uint32_t runtimeID)
{
    for (int u = 0; u < 2; ++u)
    {
        auto& hand = m_myHands[u].cards;
        auto it = std::find_if(hand.begin(), hand.end(),
            [&](const ClientCardInfo& c)
            {
                return c.runtimeID == runtimeID;
            });

        if (it != hand.end())
        {
            hand.erase(it);
            break;
        }
    }

    m_CardRuntimeIDs.erase(runtimeID);
}

uint32_t GameManager::GetMyCardRuntimeID(int unitSlot, int index) const
{
    if (unitSlot < 0 || unitSlot >= 2)
        return 0;

    const auto& hand = m_myHands[unitSlot].cards;

    if (index < 0)
        return 0;

    return hand[index].runtimeID;
}

uint32_t GameManager::GetCardDataID(uint32_t runtimeID) const
{
    auto it = m_CardRuntimeIDs.find(runtimeID);
    if (it == m_CardRuntimeIDs.end())
        return 0;
    
    return it->second;
}

const CardData GameManager::GetCardData(uint32_t runtimeID)
{
    auto dataID = GetCardDataID(runtimeID);
    return m_cardBasicMng.GetCardData(dataID);
}

const CardEffectData* GameManager::GetCardEffectData(uint32_t runtimeID)
{
    auto dataID = GetCardDataID(runtimeID);
    return m_cardBasicMng.GetEffectData(dataID);
}

const RangeData* GameManager::GetRangeData(uint32_t runtimeID)
{
    auto dataID = GetCardData(runtimeID);
    return m_cardRangeMng.GetRange(dataID.m_rangeId);
}

void GameManager::SetDrawCandidates(
    const std::vector<yuno::net::packets::CardSpawnInfo>& cards)
{
    m_drawCandidates.clear();

    for (const auto& c : cards)
    {
        m_drawCandidates.push_back({
            c.runtimeID,
            c.dataID
            });
    }

    std::cout << "[Client] DrawCandidates stored. count="
        << m_drawCandidates.size() << "\n";
}
void GameManager::SendSelectCard(int index)
{
    if (index < 0 || index >= m_drawCandidates.size())
        return;

    uint32_t runtimeID = m_drawCandidates[index].runtimeID;

    yuno::net::packets::C2S_SelectCard pkt{};
    pkt.runtimeID = runtimeID;

    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_SelectCard,
        [&](yuno::net::ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));
}
const std::vector<ClientCardInfo>& GameManager::GetDrawCandidates() const
{
    return m_drawCandidates;
}
void GameManager::ClearDrawCandidates()
{
    m_drawCandidates.clear();
}

bool GameManager::PushCardCommand(const CardPlayCommand& cmd)
{
    return m_cardQueue.Push(cmd);
}

void GameManager::ClearCardQueue()
{
    m_cardQueue.Clear();
}

bool GameManager::IsCardQueueEmpty() const
{
    return m_cardQueue.IsEmpty();
}

bool GameManager::IsCardQueueFull() const
{
    return m_cardQueue.IsFull();
}

const std::vector<CardPlayCommand>& GameManager::GetCardQueue() const
{
    return m_cardQueue.Get();
}

void GameManager::SendEmote(uint8_t emoteId)
{
    yuno::net::packets::C2S_Emote pkt{};
    pkt.emoteId = emoteId;

    auto bytes = yuno::net::PacketBuilder::Build(
        yuno::net::PacketType::C2S_Emote,
        [&](yuno::net::ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));

    std::cout << "[Client] Send Emote emoteId=" << int(emoteId) << "\n";
}

void GameManager::PushEmote(uint8_t pid, uint8_t emoteId)
{
    m_pendingEmotes.push({ pid, emoteId });
}

bool GameManager::PopEmote(PendingEmote& out)
{
    if (m_pendingEmotes.empty())
        return false;

    out = m_pendingEmotes.front();
    m_pendingEmotes.pop();
    return true;
}

bool GameManager::IsCountdownActive() const
{
    return m_countdownActive;
}

bool GameManager::IsCountdownFinished() const
{
    return m_countdownFinished;
}

int GameManager::GetCountdownNumber() const
{
    if (!m_countdownActive)
        return -1;

    return static_cast<int>(std::ceil(m_countdownRemaining));
}

int GameManager::GetCountDownSlotUnitId(int slotIndex, int unitIndex) const
{
    if (unitIndex < 0 || unitIndex > 1)
        return 0;

    switch (slotIndex)
    {
    case 1:
        return (unitIndex == 0) ? m_S1U1 : m_S1U2;
    case 2:
        return (unitIndex == 0) ? m_S2U1 : m_S2U2;
    default:
        return 0;
    }
}


void GameManager::StartCountDown(int countTime, int S1U1, int S1U2, int S2U1, int S2U2)
{
    if (m_state == CurrentSceneState::CountDown)
        return;
    // 상대방 슬롯에 전달받은 유닛ID에 맞는 텍스쳐로 변경하기 넣으면 됨
    // + 카운트 다운 3초 시작
    // 3초 후에 RoundStart씬으로 이동하면서 데이터초기화
    m_S1U1 = S1U1;
    m_S1U2 = S1U2;
    m_S2U1 = S2U1;
    m_S2U2 = S2U2;

    m_countdownActive = true;
    m_countdownFinished = false;
    m_countdownRemaining = static_cast<float>(countTime);

    SetSceneState(CurrentSceneState::CountDown);
}

void GameManager::Tick(float dt)
{
    if (m_countdownActive)
    {
        m_countdownRemaining -= dt;
        if (m_countdownRemaining <= 0.0f)
        {
            m_countdownActive = false;
            m_countdownFinished = true;
            m_countdownRemaining = 0.0f;
    
            // 여기 말고 CountDownScene에서 호출함
            //SetSceneState(CurrentSceneState::RoundStart);
        }
    }
}

void GameManager::SendPacket(std::vector<std::uint8_t>&& bytes)
{
    if (!m_clientNet)
    {
        std::cout << "[GameManager] Client network not bound.\n";
        return;
    }

    m_clientNet->SendPacket(std::move(bytes));
}

void GameManager::SetMyPick(int index, PieceType type)
{
    if (index < 0 || index >= 2)
        return;

    m_myPick[index] = type;
    m_lastPickedPiece = type;
}

void GameManager::ResetMyPicks()
{
    m_myPick[0] = PieceType::None;
    m_myPick[1] = PieceType::None;
    m_lastPickedPiece = PieceType::None;
    m_isReady = false;
    m_p1Ready = false;
    m_p2Ready = false;
}


bool GameManager::HasTwoPicks() const
{
    return (m_myPick[0] != PieceType::None) && (m_myPick[1] != PieceType::None);
}

bool GameManager::ToggleReady()
{
    m_isReady = !m_isReady;

    if (m_PID == 1)
        m_p1Ready = m_isReady;
    else if (m_PID == 2)
        m_p2Ready = m_isReady;

    return m_isReady;
}

void GameManager::SetReadyStates(bool p1Ready, bool p2Ready)
{
    m_p1Ready = p1Ready;
    m_p2Ready = p2Ready;

    if (m_PID == 1)
        m_isReady = m_p1Ready;
    else if (m_PID == 2)
        m_isReady = m_p2Ready;
}

bool GameManager::IsOpponentReady() const
{
    if (m_PID == 1)
        return m_p2Ready;
    if (m_PID == 2)
        return m_p1Ready;
    return false;
}

void GameManager::SubmitTurn(
    const std::vector<CardPlayCommand>& commands)
{
    using namespace yuno::net;

    packets::C2S_ReadyTurn pkt;
    pkt.commands = commands;

    auto bytes = PacketBuilder::Build(
        PacketType::C2S_ReadyTurn,
        [&](ByteWriter& w)
        {
            pkt.Serialize(w);
        });

    m_clientNet->SendPacket(std::move(bytes));
}


//void GameManager::RoundInit(yuno::net::packets::S2C_Error data)
//{
//    // 너가 패킷 사용해서 하고싶은거하면돼
//    // 데이터 어디 담아두는 멤버 만들어서 저장만했다가
//    // 그리드 GM.GetErrorPkt();
//    //
//}
