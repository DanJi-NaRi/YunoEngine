#pragma once
#include "EffectTemplate.h"
#include "Effect.h"

class IRenderer;

class EffectManager;
class IEffectManager
{
public:
    virtual ~IEffectManager() = default;
    virtual Effect* Spawn(EffectID id, const XMFLOAT3& pos, const XMFLOAT3& scale = { 1, 1, 1 }, const XMFLOAT3& dir = { 1, 0, 0 }) = 0;
};

class EffectManager : public IEffectManager
{
private:
    std::vector<std::unique_ptr<Effect>> m_pool;
    std::vector<Effect*> m_active;
    std::queue<Effect*> m_free;

    std::unordered_map<EffectID, EffectTemplate> m_templates;

    IRenderer* m_pRenderer;

    bool isPlay = true;
public:
    EffectManager();
    virtual ~EffectManager();

    void Init(int count);

    bool RegisterEffect(const EffectDesc& desc);

    void SetEmissive(EffectID id, float emissive);
    
    Effect* Spawn(EffectID id, const XMFLOAT3& pos, const XMFLOAT3& scale = {1, 1, 1}, const XMFLOAT3& dir = {1, 0, 0}) override;

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
