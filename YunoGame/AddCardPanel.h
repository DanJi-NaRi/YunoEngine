#pragma once
#include "Widget.h"

class TextureImage;
class OptionButton;
struct ClientCardInfo;

class AddCardPanel final : public Widget
{
public:
    explicit AddCardPanel(UIFactory& factory);

    bool Create(
        const std::wstring& name,
        uint32_t id,
        Float2 sizePx,
        XMFLOAT3 vPos,
        float rotZ,
        XMFLOAT3 vScale
    ) override;

    void CreateChild() override;
    bool Update(float dt) override;

    void Show();
    void Hide();

    void SetCandidateCards(const std::vector<ClientCardInfo>& cards);
    void OnCardSelected(int index);
private:
    TextureImage* m_bg;
    
    std::vector<OptionButton*> m_addCard;
    std::vector<ClientCardInfo> m_candidates;

    int m_selectedIndex = -1;

    bool m_locked = false;       // 클릭 잠금
    float m_closeTimer = 0.f;    // 닫기 타이머
    float m_closeDelay = 3.f;    // 3초 유지
};
