#pragma once

template <typename T>
class EffectPool
{
public:
    std::vector<T*> m_pEffects;

public:
    void Init(int count)
    {
        pool.reserve(count);

        for (int i = 0; i < count; i++)
        {
            m_pool.push_back(std::make_unique<T>());
            m_pEffects.push_back(pool.back().get());
        }
    }

    T* Get()
    {
        if (m_pEffects.empty())
            return nullptr;

        T* obj = m_pEffects.back();
        m_pEffects.pop_back();
        return obj;
    }

    void Release(T* obj)
    {
        m_pEffects.push_back(obj);
    }
};
