#pragma once
#include "EffectTemplate.h"
#include "Effect.h"

class IRenderer;

struct EffectDesc
{
    EffectID id = EffectID::Default;
    ShaderId shaderid = ShaderId::EffectBase;
    BillboardMode billboard = BillboardMode::ScreenAligned;
    float lifetime = 1.0f;
    float emissive = 0.5f;
    XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT3 rot = {0, 0, 0};
    int framecount = 0;
    int cols = 0;
    int rows = 0;
    std::wstring texPath = L"";
};

class EffectManager
{
private:
    std::vector<std::unique_ptr<Effect>> m_pool;
    std::vector<Effect*> m_active;
    std::vector<Effect*> m_free;

    std::unordered_map<EffectID, EffectTemplate> m_templates;

    IRenderer* m_pRenderer;

    bool isPlay = true;
public:
    EffectManager();
    virtual ~EffectManager();

    void Init(int count);

    bool RegisterEffect(const EffectDesc& desc);

    void SetEmissive(EffectID id, float emissive);
    
    Effect* Spawn(EffectID id, const XMFLOAT3& pos, const XMFLOAT3& scale = {1, 1, 1}, const XMFLOAT3& dir = {1, 0, 0});

    void Play();
    void Stop();

    void Update(float dt);

    void Submit(float dt);

#ifdef _DEBUG
    void Serialize();

private:
    EffectID m_selectedTemplate = EffectID::Count;
#endif
};
