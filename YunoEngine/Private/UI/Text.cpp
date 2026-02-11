#include "pch.h"
#include "Text.h"

Text::Text(UIFactory& uiFactory) : Widget(uiFactory)
{
    
}

Text::~Text() = default;

bool Text::CreateMaterial()
{
    return false;
}

bool Text::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

    if (m_MeshNode)
        m_MeshNode->m_Meshs.clear();

    return true;
}

bool Text::Start()
{
    return true;
}

bool Text::UpdateTransform(float dTime)
{
    Widget::UpdateTransform(dTime);
    return true;
}

bool Text::Update(float dTime)
{
    Widget::Update(dTime);
    return true;
}

bool Text::Submit(float dTime)
{
    if (m_pRenderer && !m_desc.text.empty())
    {
        const float finalW = m_size.x * m_finalScale.x;
        const float finalH = m_size.y * m_finalScale.y;

        m_desc.position = XMFLOAT2(m_finalPos.x + m_textOffset.x,
            m_finalPos.y + m_textOffset.y);

        m_desc.rotation = m_vRot.z;
        m_desc.scale = { m_finalScale.x, m_finalScale.y }; // 보통 이게 정답
        m_desc.origin = XMFLOAT2(m_pivot.x * finalW,
            m_pivot.y * finalH);

        m_desc.layerDepth = 0.0f;
        m_pRenderer->SubmitText(m_desc);
    }

    return Widget::Submit(dTime);
}
