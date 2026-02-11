#include "pch.h"
#include "AddCardPanel.h"
#include "YunoEngine.h"
#include "UIFactory.h"
#include "TextureImage.h"
#include "OptionButton.h"
#include "GameManager.h"

AddCardPanel::AddCardPanel(UIFactory& factory)
    : Widget(factory)
{
}

bool AddCardPanel::Create(
    const std::wstring& name,
    uint32_t id,
    Float2 sizePx,
    XMFLOAT3 vPos,
    float rotZ,
    XMFLOAT3 vScale
)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);
    //CreateMaterial(L"../Assets/UI/TITLE/volume_unfill.png");
    return true;
}

void AddCardPanel::CreateChild()
{
    // 배경
    m_bg = m_uiFactory.CreateChild<TextureImage>(
        L"AddCardBG",
        Float2{ 3840,2160 },
        XMFLOAT3(960, 540, 0),
        UIDirection::Center,
        this
    );
    m_bg->ChangeTexture(L"../Assets/UI/PLAY/cardselect.png");
    // 버튼 3개 생성
    for (int i = 0; i < 3; ++i)
    {
        auto* btn = m_uiFactory.CreateChild<OptionButton>(
            L"AddCardBtn_" + std::to_wstring(i),
            Float2{ 205/2,297/2 },
            XMFLOAT3(810 + i * 150, 540, 0),
            UIDirection::Center,
            this
        );

        btn->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");

        int index = i;
        btn->SetEventLMB([this, index]()
            {
                OnCardSelected(index);
            });

        m_addCard.push_back(btn);
    }
}

void AddCardPanel::SetCandidateCards(const std::vector<ClientCardInfo>& cards)
{
    m_candidates = cards;

    for (auto* btn : m_addCard)
    {
        btn->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        btn->SetScale({ 1.f,1.f,1.f });
    }

    m_selectedIndex = -1;
}

void AddCardPanel::OnCardSelected(int index)
{
    if (m_locked) return;

    if (index < 0 || index >= m_candidates.size())
        return;

    m_locked = true;
    m_selectedIndex = index;
    m_closeTimer = 0.f;

    for (auto* btn : m_addCard)
    {
        btn->ChangeTexture(L"../Assets/UI/CARD/Card_back.png");
        btn->SetScale({ 1.f,1.f,1.f });
    }
    // 선택한 카드만 강조 가능 (선택 연출)
    uint32_t dataID = m_candidates[index].dataID;

    std::wstring texturePath =
        GameManager::Get()
        .GetCardBasicManager()
        .GetCardTexturePath(dataID);

    m_addCard[index]->ChangeTexture(texturePath);
    m_addCard[index]->SetScale({ 1.2f,1.2f,1.2f });
    
    GameManager::Get().SendSelectCard(index);

    std::cout << "[Client] Select card index="
        << index
        << " runtimeID="
        << m_candidates[index].runtimeID
        << "\n";
}

void AddCardPanel::Show()
{
    SetVisible(Visibility::Visible);

    if (m_bg)
        m_bg->SetVisible(Visibility::Visible);

    for (auto* btn : m_addCard)
    {
        if (btn)
            btn->SetVisible(Visibility::Visible);
    }
}

void AddCardPanel::Hide()
{
    SetVisible(Visibility::Collapsed);

    if (m_bg)
        m_bg->SetVisible(Visibility::Collapsed);

    for (auto* btn : m_addCard)
    {
        if (btn)
            btn->SetVisible(Visibility::Collapsed);
    }
}

bool AddCardPanel::Update(float dt)
{
    return Widget::Update(dt);
    if (m_locked)
    {
        m_closeTimer += dt;

        if (m_closeTimer >= m_closeDelay)
        {
            Hide();

            // 상태 리셋
            m_locked = false;
            m_selectedIndex = -1;
        }
    }
}

