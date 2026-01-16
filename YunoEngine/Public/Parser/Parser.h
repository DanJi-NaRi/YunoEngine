#pragma once
#include "RenderTypes.h"
#include "Singleton.h"
#include "Animator.h"

class Parser : public Singleton<Parser>
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    std::pair<std::unique_ptr<MeshNode>, std::unique_ptr<Animator>> LoadFile(const std::wstring& filename);
};
