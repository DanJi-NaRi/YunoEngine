#include "pch.h"

#include "YunoLight.h"
#include "UImgui.h"


const DirectX::XMFLOAT4& YunoDirectionalLight::GetDirFloat4Reverse()
{

    return XMFLOAT4(-m_desc.direction.x, -m_desc.direction.y, -m_desc.direction.z, -m_desc.direction.w);
}

const XMFLOAT4& YunoDirectionalLight::GetDirFloat4()
{
      return m_desc.direction; 
}

const DirectX::XMFLOAT3& YunoDirectionalLight::GetDirFloat3()
{
    return XMFLOAT3(m_desc.direction.x, m_desc.direction.y, m_desc.direction.z);
}

void YunoDirectionalLight::SetDirection(const XMFLOAT4& dir)
{
    m_desc.direction = dir;
}

void YunoDirectionalLight::SetDirection(const XMFLOAT3& dir)
{
    m_desc.direction.x = dir.x;
    m_desc.direction.y = dir.y;
    m_desc.direction.z = dir.z;
}

const DirectX::XMFLOAT4& YunoDirectionalLight::GetDiffFloat4()
{
    return m_desc.diff;
}

const DirectX::XMFLOAT3& YunoDirectionalLight::GetDiffFloat3()
{
    return XMFLOAT3(m_desc.diff.x, m_desc.diff.y, m_desc.diff.z);
}

void YunoDirectionalLight::SetDiff(const XMFLOAT4& diff)
{
    m_desc.diff = diff;
    m_desc.amb.x = diff.x / 5;
    m_desc.amb.y = diff.y / 5;
    m_desc.amb.z = diff.z / 5;
}

void YunoDirectionalLight::SetDiff(const XMFLOAT3& diff)
{
    m_desc.diff.x = diff.x;
    m_desc.diff.y = diff.y;
    m_desc.diff.z = diff.z;
    m_desc.amb.x = diff.x / 5;
    m_desc.amb.y = diff.y / 5;
    m_desc.amb.z = diff.z / 5;
}

const DirectX::XMFLOAT4& YunoDirectionalLight::GetAmbFloat4()
{
    return m_desc.amb;
}

const DirectX::XMFLOAT3& YunoDirectionalLight::GetAmbFloat3()
{
    return XMFLOAT3(m_desc.amb.x, m_desc.amb.y, m_desc.amb.z);
}

const DirectX::XMFLOAT4& YunoDirectionalLight::GetSpecFloat4()
{
    return m_desc.spec;
}

const DirectX::XMFLOAT3& YunoDirectionalLight::GetSpecFloat3()
{
    return XMFLOAT3(m_desc.spec.x, m_desc.spec.y, m_desc.spec.z);
}

void YunoDirectionalLight::SetSpec(const XMFLOAT4& spec)
{
    m_desc.spec = spec;
}

void YunoDirectionalLight::SetSpec(const XMFLOAT3& spec)
{
    m_desc.spec.x = spec.x;
    m_desc.spec.y = spec.y;
    m_desc.spec.z = spec.z;
}

float YunoDirectionalLight::GetIntensity()
{
    return m_desc.intensity;
}

void YunoDirectionalLight::SetIntensity(float i)
{
    m_desc.intensity = i;
}

const DirectionalLightDesc& YunoDirectionalLight::GetDesc()
{
    return m_desc;
}



void YunoDirectionalLight::SetDesc(const DirectionalLightDesc& d)
{
    m_desc = d;
}


#ifdef _DEBUG
void YunoDirectionalLight::Serialize()
{
    if (UI::CollapsingHeader("DirectionalLight"))
    {
        if (UI::DragFloat("Intensity", &m_desc.intensity, 0.01f, 0.0f, 10.0f))
        {

        }
        if (UI::DragFloat3Editable("LightColor", &m_desc.diff.x, 0.001f, 0.0f, 1.0f))
        {

        }
    }
}
#endif

YunoPointLight::~YunoPointLight()
{
}




void YunoPointLight::SetPos(const XMFLOAT3& pos) 
{ 
    m_desc.lightpos = pos; 
}

const XMFLOAT3& YunoPointLight::GetPos() 
{ 
    return m_desc.lightpos; 
}

void YunoPointLight::SetLightColor(const XMFLOAT4& col)
{
    m_desc.lightCol = col;
}

const XMFLOAT4& YunoPointLight::GetLightColor()
{
    return m_desc.lightCol;
}

void YunoPointLight::SetIntensity(const float& i)
{
    m_desc.intensity = i;
}

float YunoPointLight::GetIntensity()
{
    return m_desc.intensity;
}

void YunoPointLight::SetDesc(const PointLightDesc& d)
{
    m_desc = d;
}

#ifdef _DEBUG
void YunoPointLight::Serialize()
{
    if (UI::CollapsingHeader("PointLight"))
    {
        if (UI::CollapsingHeader("Transform"))
        {
            auto& pos = m_desc.lightpos;

            static bool editPos = false;
            static float value = 0.0f;

            UI::Separator();

            if (!editPos)
            {
                if (UI::DragFloat3("Position##Drag", &pos.x, 0.1f))
                {
                }

                if (UI::IsItemHovered() &&
                    UI::IsLeftMouseDoubleClicked())
                {
                    editPos = true;
                }
            }
            else
            {
                if (UI::InputFloat3("Position##Input", &pos.x))
                {
                }

                if (UI::IsItemDeactivatedAfterEdit())
                {
                    editPos = false;
                }
            }
            if (UI::CollapsingHeader("LightColor"))
            {
                if (UI::DragFloat3Editable("LightColor", (float*)&m_desc.lightCol.x, 0.001f, 0.0f, 1.0f))
                {

                }
            }
            if (UI::DragFloat("Intensity", &m_desc.intensity, 0.1f, 0.0f, 1000.0f, "%.3f"))
            {

            }
        }
    }
}
#endif

const PointLightDesc& YunoPointLight::GetDesc()
{
    return m_desc;
}
