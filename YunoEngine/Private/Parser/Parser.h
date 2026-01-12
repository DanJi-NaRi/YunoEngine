#pragma once
#include "RenderTypes.h"
#include "Singleton.h"
#include "Mesh.h"

struct aiMesh;
struct aiScene;
struct aiNode;

struct MeshNode {
    MeshNode* m_Parent = nullptr;
    std::vector<std::unique_ptr<MeshNode>> m_Childs;

    std::vector<std::unique_ptr<Mesh>> m_Meshs;
};

class Parser : public Singleton<Parser>
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    std::unique_ptr<MeshNode> LoadFile(const std::string& filename);

private:
    std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene);
    std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene);
};
