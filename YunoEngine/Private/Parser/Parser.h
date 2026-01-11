#pragma once
#include "RenderTypes.h"
#include "Singleton.h"

struct aiMesh;
struct aiScene;
struct aiNode;

class Parser : public Singleton<Parser>
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    bool LoadFile(const std::string& filename);

private:
    void CreateNode(aiNode* node, const aiScene* scene);
    std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene);
};
