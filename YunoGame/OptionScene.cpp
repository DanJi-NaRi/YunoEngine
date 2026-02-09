#include "pch.h"
#include "OptionScene.h"

#include "YunoEngine.h"
#include "GameManager.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"
#include "OptionButton.h"
#include "VolumePanel.h"

bool OptionScene::OnCreateScene()
{
    // UI 전용 씬
    m_uiManager->SetOrthoFlag(true);

    auto win = YunoEngine::GetWindow();
    ClientW = (float)win->GetClientWidth();
    ClientH = (float)win->GetClientHeight();

    CreateWidget<TextureImage>(L"Option", L"../Assets/UI/TITLE/option.png", XMFLOAT3(0, 0, 0));

    CreateMainUI();
    CreateVolumeUI();
    CreateCreditUI();

    ChangeUIState(OptionUIState::Main);

    return true;
}

void OptionScene::CreateMainUI()
{
    const float baseX = (ClientW - 630) / 2;
    const float baseY = ClientH / 2;

    auto makePos = [&](int index)
        {
            return XMFLOAT3(
                baseX,
                baseY - 200.f + index * 100.f,
                0.f
            );
        };

    // Volume 버튼//TODO :: 위치 확인
    m_volumeBtn = CreateWidget<OptionButton>(
        L"VolumeBtn",
        Float2(212, 45),
        makePos(0),
        UIDirection::Center
    );

    m_volumeBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/volume_mouseout.png",
        L"../Assets/UI/TITLE/volume_mouseover.png"
    );

    m_volumeBtn->SetOnClick([this]()
        {
            ChangeUIState(OptionUIState::Volume);
        });

    // Credit 버튼
    m_creditBtn = CreateWidget<OptionButton>(
        L"CreditBtn",
        Float2(212, 45),
        makePos(1),
        UIDirection::Center
    );

    m_creditBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/credit_mouseout.png",
        L"../Assets/UI/TITLE/credit_mouseover.png"
    );

    m_creditBtn->SetOnClick([this]()
        {
            ChangeUIState(OptionUIState::Credit);
        });

    //Back//TODO :: 위치 확인
    m_backBtn = CreateWidget<SceneChangeButton>(
        L"BackBtn",
        Float2(450, 90),
        XMFLOAT3(ClientW / 2 + 308, ClientH / 2 + 350, 0),
        UIDirection::Center
    );
    m_backBtn->SetTargetScene(CurrentSceneState::Title);
    m_backBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/back_mouseout.png",
        L"../Assets/UI/TITLE/back_mouseover.png"
    );
}

void OptionScene::CreateVolumeUI()
{
    const float centerX = ClientW / 2.f + 320;
    const float centerY = ClientH / 2.f - 50;

    constexpr float gapY = 110.f;   // 게이지 간 간격
    //TODO :: 위치 확인
    for (int i = 0; i < 3; ++i)
    {
        m_volumePanels[i] = CreateWidget<VolumePanel>(
            L"VolumePanel",
            Float2(600, 200),
            XMFLOAT3(
                centerX,
                centerY + gapY - i * gapY,
                0.f
            )
        );
        m_volumePanels[i]->ChangeTexture(L"../Assets/UI/TITLE/volume_fill_full.png");
        m_volumePanels[i]->SetPivot(UIDirection::Center);
        m_volumePanels[i]->SetVisible(Visibility::Hidden);
    }
}

void OptionScene::CreateCreditUI()
{
    m_creditRoot = CreateWidget<TextureImage>(//TODO :: 위치 확인
        L"CreditPanel",
        L"../Assets/UI/TITLE/option_credit.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 + 100, 0),
        UIDirection::Center
    );
    m_creditRoot->SetVisible(Visibility::Hidden);
}


void OptionScene::ChangeUIState(OptionUIState state)
{
    // 숨김
    m_creditRoot->SetVisible(Visibility::Hidden);

    for (auto* panel : m_volumePanels)
        panel->SetVisible(Visibility::Hidden);

    m_uiState = state;

    switch (state)
    {
    case OptionUIState::Main:
        std::cout << "\nMain\n";
        m_volumeBtn->SetVisible(Visibility::Visible);
        m_creditBtn->SetVisible(Visibility::Visible);
        break;

    case OptionUIState::Volume:
        std::cout << "\nVolume\n";
        for (auto* panel : m_volumePanels)
            panel->SetVisible(Visibility::Visible);
        break;

    case OptionUIState::Credit:
        std::cout << "\nCredit\n";
        m_creditRoot->SetVisible(Visibility::Visible);
        break;
    }
}

void OptionScene::OnDestroyScene()
{
}

void OptionScene::OnEnter()
{
    YunoEngine::GetInput()->AddContext(&m_uiCtx, this);
}

void OptionScene::OnExit()
{
    YunoEngine::GetInput()->RemoveContext(&m_uiCtx);
}

void OptionScene::Update(float dt)
{
    SceneBase::Update(dt);

}

void OptionScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void OptionScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
