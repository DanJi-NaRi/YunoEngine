#include "pch.h"

#include "ObjectManager.h"
#include "ObjectTypeRegistry.h"

//using CreateFn = std::function<void(ObjectManager&, const UnitDesc&)>;

void ObjectTypeRegistry::Register(const std::wstring& type, ObjectTypeRegistry::CreateFn fn)
{
    m_map.emplace(type, fn);
}

ObjectTypeRegistry::CreateFn* ObjectTypeRegistry::Find(const std::wstring& type)
{
    auto it = m_map.find(type);
    if (it == m_map.end()) return nullptr;
    return &it->second;
}
