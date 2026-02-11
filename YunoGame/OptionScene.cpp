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

    CreateWidget<TextureImage>(L"StandByBackGround", L"../Assets/UI/WEAPON_SELECT/black_background.png", XMFLOAT3(ClientW / 2, ClientH / 2, 0), UIDirection::Center);

    CreateWidget<TextureImage>(L"Option", L"../Assets/UI/TITLE/option.png", XMFLOAT3(0, 0, 0));

    CreateMainUI();
    CreateVolumeUI();
    CreateCreditUI();

    ChangeUIState(OptionUIState::Main);

    return true;
}

void OptionScene::CreateMainUI()
{
    const float baseX = ((ClientW - 630) / 2) - 2;
    const float baseY = ClientH / 2 - 95;

    auto makePos = [&](int index)
        {
            return XMFLOAT3(
                baseX,
                baseY - 200.f + index * 115,
                0.f
            );
        };

    // Volume 버튼//TODO :: 위치 확인
    m_volumeBtn = CreateWidget<OptionButton>(
        L"VolumeBtn",
        Float2(208.5f, 45),
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
        Float2(208.5f, 45),
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
    m_backBtn = CreateWidget<PopButton>(
        L"BackBtn",
        Float2(416, 90),
        XMFLOAT3(ClientW / 2 + 315, ClientH / 2 + 350, 0),
        UIDirection::Center
    );
    m_backBtn->SetHoverTexture(
        L"../Assets/UI/TITLE/back_mouseout.png",
        L"../Assets/UI/TITLE/back_mouseover.png"
    );
}

void OptionScene::CreateVolumeUI()
{
    const float centerX = ClientW / 2.f + 100.0f;
    const float centerY = ClientH / 2.f - 145.0f;

    constexpr float gapY = 115;   // 게이지 간 간격
    //TODO :: 위치 확인
    for (int i = 0; i < 3; ++i)
    {
        m_volumePanels[i] = CreateWidget<VolumePanel>(
            L"VolumePanel",
            Float2(600, 80),
            XMFLOAT3(
                centerX,
                centerY + gapY - i * gapY,
                0.f
            )
        );

        m_volumePanels[i]->SetVolumeType(i);
        m_volumePanels[i]->ChangeTexture(L"../Assets/UI/TITLE/volume_fill_full.png");
        m_volumePanels[i]->SetPivot(UIDirection::Center);
        m_volumePanels[i]->SetVisible(Visibility::Visible);
    }
    m_volumeRoot = CreateWidget<TextureImage>(//TODO :: 위치 확인
        L"CreditPanel",
        L"../Assets/UI/TITLE/option_volume.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 , 0),
        UIDirection::Center
    );
    m_volumeRoot->SetVisible(Visibility::Visible);

    m_volumePanels[0]->SetLevel(GameManager::Get().GetMasterVolume());
    m_volumePanels[1]->SetLevel(GameManager::Get().GetBGMVolume());
    m_volumePanels[2]->SetLevel(GameManager::Get().GetSFXVolume());
}

void OptionScene::CreateCreditUI()
{
    m_creditRoot = CreateWidget<TextureImage>(//TODO :: 위치 확인
        L"CreditPanel",
        L"../Assets/UI/TITLE/option_credit_ver2.png",
        XMFLOAT3(ClientW / 2, ClientH / 2 + 20, 0),
        UIDirection::Center
    );
    m_creditRoot->SetVisible(Visibility::Hidden);
}

void OptionScene::ChangeUIState(OptionUIState state)
{
    // 숨김
    m_creditRoot->SetVisible(Visibility::Hidden);
    m_volumeRoot->SetVisible(Visibility::Hidden);
    m_volumeBtn->SetSelected(false);
    m_creditBtn->SetSelected(false);

    for (auto* panel : m_volumePanels)
        panel->SetVisible(Visibility::Collapsed);

    m_uiState = state;

    switch (state)
    {
    case OptionUIState::Main:
        m_volumeBtn->SetVisible(Visibility::Visible);
        m_creditBtn->SetVisible(Visibility::Visible);
        m_volumeRoot->SetVisible(Visibility::Visible);
        m_volumeBtn->SetSelected(true);
        for (auto* panel : m_volumePanels)
            panel->SetVisible(Visibility::Visible);
        break;

    case OptionUIState::Volume:
        m_volumeBtn->SetSelected(true);
        m_volumeRoot->SetVisible(Visibility::Visible);
        for (auto* panel : m_volumePanels)
            panel->SetVisible(Visibility::Visible);
        break;

    case OptionUIState::Credit:
        m_creditBtn->SetSelected(true);
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
