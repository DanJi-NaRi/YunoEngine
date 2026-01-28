#pragma once

#include "Singleton.h"

class ObjectManager;
struct UnitDesc;
class Unit;

class ObjectTypeRegistry : public Singleton<ObjectTypeRegistry>
{
public:
    using CreateFn = std::function<void(ObjectManager&, const UnitDesc&)>;
    void Register(const std::wstring& type, CreateFn fn) {
        m_map.emplace(type, fn);
    }

    CreateFn* Find(const std::wstring& type);

private:
    std::unordered_map<std::wstring, CreateFn> m_map;
};


