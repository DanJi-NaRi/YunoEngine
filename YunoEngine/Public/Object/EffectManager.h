#pragma once
#include "EffectTemplate.h"
#include "EffectPool.h"
#include "Effect.h"

class IRenderer;

class EffectManager
{
private:
    std::vector<std::unique_ptr<Effect>> m_pool;
    std::vector<Effect*> m_active;
    std::vector<Effect*> m_free;

    std::unordered_map<EffectID, EffectTemplate> m_templates;

    IRenderer* m_pRenderer;

public:
    EffectManager();
    virtual ~EffectManager();

    void Init(int count);

    bool RegisterEffect(EffectID id, ShaderId shaderid, BillboardMode mode, float lifetime, int framecount, int cols, int rows, const std::wstring& texturePath);
    
    Effect* Spawn(EffectID id, const XMFLOAT3& pos, const XMFLOAT3& scale = {1, 1, 1}, const XMFLOAT3& dir = {1, 0, 0});

    void Update(float dt);

    void Submit(float dt);
};
