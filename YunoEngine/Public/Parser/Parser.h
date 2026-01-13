#pragma once
#include "RenderTypes.h"
#include "Singleton.h"

class Parser : public Singleton<Parser>
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    std::unique_ptr<MeshNode> LoadFile(const std::wstring& filename);
};
