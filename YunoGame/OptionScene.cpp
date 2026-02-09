#include "pch.h"
#include "OptionScene.h"

#include "YunoEngine.h"
#include "UIManager.h"
#include "IWindow.h"
#include "UIWidgets.h"
#include "GameManager.h"


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
    //CreateCreditUI();

    ChangeUIState(OptionUIState::Main);

    return true;
}

void OptionScene::CreateMainUI()
{
    const float baseX = ClientW / 2 - 125.f;
    const float baseY = ClientH / 2;

    auto makePos = [&](int index)
        {
            return XMFLOAT3(
                baseX,
                baseY - 200.f + index * 100.f,
                0.f
            );
        };

    // Volume 버튼
    m_volumeBtn = CreateWidget<OptionButton>(
        L"VolumeBtn",
        Float2(212, 45),
        makePos(0),
        UIDirection::Center
    );

    m_volumeBtn->SetCursurTexture(
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

    m_creditBtn->SetCursurTexture(
        L"../Assets/UI/TITLE/credit_mouseout.png",
        L"../Assets/UI/TITLE/credit_mouseover.png"
    );

    m_creditBtn->SetOnClick([this]()
        {
            ChangeUIState(OptionUIState::Credit);
        });
}

void OptionScene::CreateVolumeUI()
{
    m_volumeRoot = CreateWidget<Widget>(
        L"VolumeRoot",
        Float2(600, 200),
        XMFLOAT3(ClientW / 2, ClientH / 2, 0)
    );
    m_volumeRoot->SetPivot(UIDirection::Center);
    m_volumeRoot->

    // === Track (입력 전용) ===
    m_volumeTrack = CreateWidget<VolumeTrackButton>(
        L"VolumeTrack",
        Float2(20 * 22.f, 40),
        XMFLOAT3(0, 0, 0)
    );
    m_volumeTrack->Attach(m_volumeRoot);

    // === Steps ===
    constexpr int kSteps = 20;
    constexpr float gap = 22.f;
    float startX = -((kSteps - 1) * gap) * 0.5f;

    for (int i = 0; i < kSteps; ++i)
    {
        auto* img = CreateWidget<Widget>(
            L"VolumeStep",
            Float2(20, 40),
            XMFLOAT3(startX + gap * i, 0, 0)
        );
        img->ChangeTexture(L"../Assets/UI/TITLE/volume_unfill.png");
        img->Attach(m_volumeRoot);
        m_volumeSteps.push_back(img);
    }

    //Back
    m_backBtn = CreateWidget<OptionButton>(
        L"BackBtn",
        Float2(120, 40),
        XMFLOAT3(ClientW / 2, ClientH / 2 + 150, 0),
        UIDirection::Center
    );
    m_backBtn->SetCursurTexture(
        L"../Assets/UI/TITLE/back_mouseout.png",
        L"../Assets/UI/TITLE/back_mouseover.png"
    );
    //m_backBtn->SetVisible(false);

    m_backBtn->SetOnClick([this]()
        {
            ChangeUIState(OptionUIState::Main);
        });

    //SetVolume(m_volumeLevel);
}

void OptionScene::CreateCreditUI()
{
    m_creditRoot = CreateWidget<TextureImage>(
        L"CreditPanel",
        L"../Assets/UI/TITLE/option_credit.png",
        XMFLOAT3(ClientW / 2, ClientH / 2, 0),
        UIDirection::Center
    );
    //m_creditRoot->SetVisible(false);
}

void OptionScene::ChangeUIState(OptionUIState state)
{
    // 전부 숨김
    /*m_volumeBtn->SetVisible(false);
    m_creditBtn->SetVisible(false);
    m_volumeRoot->SetVisible(false);
    m_creditRoot->SetVisible(false);
    m_backBtn->SetVisible(false);*/

    m_uiState = state;

    switch (state)
    {
    case OptionUIState::Main:
        std::cout << "\nMain\n";
        /*m_volumeBtn->SetVisible(true);
        m_creditBtn->SetVisible(true);*/
        break;

    case OptionUIState::Volume:
        std::cout << "\nVolume\n";
        /*m_volumeRoot->SetVisible(true);
        m_backBtn->SetVisible(true);*/
        break;

    case OptionUIState::Credit:
        std::cout << "\nCredit\n";
        /*m_creditRoot->SetVisible(true);
        m_backBtn->SetVisible(true);*/
        break;
    }
}

//void OptionScene::UpdateVolumeByMouse(int mouseX)
//{
//    RECT rc = m_volumeTrack->GetRect();
//    float t = float(mouseX - rc.left) / float(rc.right - rc.left);
//    t = std::clamp(t, 0.f, 1.f);
//
//    int level = int(std::round(t * 20.f));
//    SetVolume(level);
//}
//
//void OptionScene::SetVolume(int level)
//{
//    m_volumeLevel = std::clamp(level, 0, 20);
//
//    for (int i = 0; i < 20; ++i)
//    {
//        m_volumeSteps[i]->ChangeTexture(
//            i < m_volumeLevel
//            ? L"../Assets/UI/TITLE/volume_fill.png"
//            : L"../Assets/UI/TITLE/volume_unfill.png"
//        );
//    }
//
//    //사운드 적용
//    //GameManager::Get().SetMasterVolume(m_volumeLevel / 20.f);
//}

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

    if (!m_volumeTrack) return;

    //auto* drag = m_volumeTrack->GetDragProvider();
    //if (!drag) return;

    //if (drag->IsNowDragging())
    //{
    //    // 마우스 X 기준으로 볼륨 갱신
    //    POINT pt{
    //        (LONG)YunoEngine::GetInput()->GetMouseX(),
    //        (LONG)YunoEngine::GetInput()->GetMouseY()
    //    };

    //    //UpdateVolumeByMouse(pt.x);
    //}
}

void OptionScene::SubmitObj()
{
    SceneBase::SubmitObj();
}

void OptionScene::SubmitUI()
{
    SceneBase::SubmitUI();
}
