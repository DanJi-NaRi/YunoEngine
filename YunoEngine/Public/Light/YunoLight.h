#pragma once


enum class LightType : uint8_t
{
    Directional = 0,
};

struct DirectionalLightDesc
{
     
    XMFLOAT4 direction = { 0.58f, -0.58f, 0.58f, 0.0f };

    XMFLOAT4 diff = { 1.0f, 1.0f, 1.0f, 1.0f };

    XMFLOAT4 amb = { 0.2f, 0.2f, 0.2f, 1.0f };

    XMFLOAT4 spec = { 1.0f, 1.0f, 1.0f, 1.0f };


    float intensity = 3.0f;

};

class Light
{
public:
    virtual ~Light() = default;
    virtual LightType GetType() const = 0;
    //virtual bool IsEnabled() const = 0;
    //virtual void SetEnabled(bool v) = 0;
};


class YunoDirectionalLight final :public Light 
{
public:
    YunoDirectionalLight() = default;
    explicit YunoDirectionalLight(const DirectionalLightDesc& d) : m_desc(d) {}

    LightType GetType() const override { return LightType::Directional; }

    // 온/오프 넣을까 말까
    //bool IsEnabled() const override { return m_desc.enabled; }
    //void SetEnabled(bool v) override { m_desc.enabled = v; }

    const XMFLOAT4& GetDirFloat4Reverse();
    const XMFLOAT4& GetDirFloat4();
    const XMFLOAT3& GetDirFloat3();
    void SetDirection(const XMFLOAT4& dir); 
    void SetDirection(const XMFLOAT3& dir);

    const XMFLOAT4& GetDiffFloat4(); 
    const XMFLOAT3& GetDiffFloat3(); 
    void SetDiff(const XMFLOAT4& diff);

    void SetDiff(const XMFLOAT3& diff);


    const XMFLOAT4& GetAmbFloat4(); 
    const XMFLOAT3& GetAmbFloat3(); 

    const XMFLOAT4& GetSpecFloat4(); 
    const XMFLOAT3& GetSpecFloat3(); 
    void SetSpec(const XMFLOAT4& spec);
    void SetSpec(const XMFLOAT3& spec);


    float GetIntensity(); 
    void SetIntensity(float i); 

    const DirectionalLightDesc& GetDesc(); 
    void SetDesc(const DirectionalLightDesc& d); 

private:
    DirectionalLightDesc m_desc{};
};
