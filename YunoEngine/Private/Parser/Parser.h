#pragma once
#include "RenderTypes.h"
#include "Singleton.h"
#include "Animator.h"

struct VertexWeight
{
    UINT boneIndex;
    float weight;
};

class MeshNode;

class Parser : public Singleton<Parser>
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    std::unique_ptr<MeshNode> LoadFile(const std::wstring& filename, PassOption opt);
    std::unique_ptr<AnimationClip> LoadAnimationClipFromFile(const std::wstring& filename);
    std::unique_ptr<Animator> LoadAnimatorFromFile(const std::string& name, const std::wstring& filename);
};
