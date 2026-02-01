#include "pch.h"
#include "Text.h"

Text::Text(UIFactory& uiFactory) : Widget(uiFactory)
{
}

Text::~Text() = default;

bool Text::CreateMaterial()
{
    return true;
}

bool Text::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    if (!Widget::Create(name, id, vPos))
        return false;

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
        m_desc.position = XMFLOAT2(m_finalPos.x + m_textOffset.x, m_finalPos.y + m_textOffset.y);
        m_desc.rotation = m_vRot.z;
        m_desc.origin = XMFLOAT2(m_pivot.x * m_finalSize.x, m_pivot.y * m_finalSize.y); //텍스트 사각형 내부에서 텍스트 위치
        m_desc.layerDepth = 0.0f;
        m_pRenderer->SubmitText(m_desc);
    }

    return Widget::Submit(dTime);
}
