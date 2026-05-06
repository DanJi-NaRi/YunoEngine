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
        XMFLOAT2 drawPos = XMFLOAT2(m_finalPos.x, m_finalPos.y);
        XMFLOAT2 drawScale = XMFLOAT2(m_finalScale.x, m_finalScale.y);
        float drawRot = m_vRot.z;

        if (m_Parent)
        {
            const XMMATRIX parentWorld = m_Parent->GetWorldNoSizeMatrix();
            const XMVECTOR localPos = XMVectorSet(m_finalPos.x, m_finalPos.y, m_finalPos.z, 1.0f);
            const XMVECTOR worldPos = XMVector3TransformCoord(localPos, parentWorld);

            drawPos.x = XMVectorGetX(worldPos);
            drawPos.y = XMVectorGetY(worldPos);

            XMVECTOR parentScale;
            XMVECTOR parentRotQ;
            XMVECTOR parentTrans;
            if (XMMatrixDecompose(&parentScale, &parentRotQ, &parentTrans, parentWorld))
            {
                XMFLOAT3 ps{};
                XMStoreFloat3(&ps, parentScale);

                drawScale.x *= ps.x;
                drawScale.y *= ps.y;

                XMFLOAT4 q{};
                XMStoreFloat4(&q, parentRotQ);

                const float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
                const float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
                drawRot += std::atan2(siny_cosp, cosy_cosp);
            }
        }

        const float finalW = m_size.x * drawScale.x;
        const float finalH = m_size.y * drawScale.y;

        m_desc.position = XMFLOAT2(drawPos.x + m_textOffset.x,
            drawPos.y + m_textOffset.y);

        m_desc.rotation = drawRot;
        m_desc.scale = drawScale;
        m_desc.origin = XMFLOAT2(m_pivot.x * finalW,
            m_pivot.y * finalH);

        m_desc.layerDepth = 0.0f;
        m_pRenderer->SubmitText(m_desc);
    }

    return Widget::Submit(dTime);
}
