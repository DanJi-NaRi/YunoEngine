#include "pch.h"

#include "YunoRenderer.h"
#include "IMesh.h"
#include "YunoEngine.h"
#include "Mesh.h"
//#include "Animator.h"

#include "Parser.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, XMMATRIX& parentTM, const std::wstring& filepath);
std::unique_ptr<Animator> CreateAnimator(aiNode* node, const aiScene* scene, std::unordered_map<std::string, UINT>& boneNameSet, std::unordered_map<std::string, XMMATRIX>& nameToOffset);
void CreateBoneNameSet(const aiScene* scene, std::unordered_map<std::string, UINT>& indexOut, std::unordered_map<std::string, XMMATRIX>& matrixOut);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::wstring& filepath);
bool CheckSkeletalModel(const aiScene* scene);
void CreateBoneNode(aiNode* node, size_t& curIndex, BoneNode* root, BoneNode* parent,
    std::unordered_map<std::string, UINT>& nameToIndex, std::unordered_map<std::string, XMMATRIX>& nameTomOffset);

std::wstring Utf8ToWString(const char* s)
{
    if (!s) return {};
    const int len = static_cast<int>(std::strlen(s));
    if (len == 0) return {};

    const int wlen = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    if (wlen <= 0) return {};

    std::wstring w;
    w.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, s, len, w.data(), wlen);
    return w;
}

std::pair<std::unique_ptr<MeshNode>, std::unique_ptr<Animator>> Parser::LoadFile(const std::wstring& filepath)
{
    Assimp::Importer importer;

    std::string pathUtf8 = std::filesystem::path(filepath).u8string();

    const aiScene* scene = importer.ReadFile(
        pathUtf8,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace  |
        aiProcess_GlobalScale
    );

    if (!scene || !scene->mRootNode)
        return {};

    bool HasSkeletal = false;

    std::unordered_map<std::string, UINT> BoneNameToIndex;
    std::unordered_map<std::string, XMMATRIX> BoneNameToOffset;

    if (scene->HasAnimations())
        if (CheckSkeletalModel(scene))
        {
            CreateBoneNameSet(scene, BoneNameToIndex, BoneNameToOffset);
            auto Animator = CreateAnimator(scene->mRootNode, scene, BoneNameToIndex, BoneNameToOffset);
        }

    XMMATRIX root = XMMatrixIdentity();

    auto MeshNode = CreateNode(scene->mRootNode, scene, 0, root, filepath);

    return std::make_pair(std::move(MeshNode), nullptr);
}

bool CheckSkeletalModel(const aiScene* scene)
{
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];

        if (mesh->HasBones())
            return true;
    }

    return false;
}

void CreateBoneNameSet(const aiScene* scene, std::unordered_map<std::string, UINT>& indexOut, std::unordered_map<std::string, XMMATRIX>& matrixOut)
{
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];

        if (mesh->HasBones())
        {
            for (size_t j = 0; j < mesh->mNumBones; j++)
            {
                std::string name(mesh->mBones[j]->mName.C_Str());
                if (indexOut.find(name) == indexOut.end())
                {
                    indexOut.emplace(name, -1);

                    XMMATRIX mBoneOffset = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&mesh->mBones[j]->mOffsetMatrix.Transpose()));
                    matrixOut.emplace(name, mBoneOffset);
                }
            }
        }
    }

    for (size_t i = 0; i < scene->mNumAnimations; i++)
    {
        auto anim = scene->mAnimations[i];

        for (size_t i = 0; i < anim->mNumChannels; i++)
        {
            std::string name(anim->mChannels[i]->mNodeName.C_Str());
            indexOut.emplace(name, -1);
        }
    }
}

std::unique_ptr<Animator> CreateAnimator(aiNode* node, const aiScene* scene, std::unordered_map<std::string, UINT>& nameToIndex, //<- Use BoneName Set Too 본 네임 집합으로도 쓰임
    std::unordered_map<std::string, XMMATRIX>& nameToOffset)
{
    std::unique_ptr<Animator> animator = std::make_unique<Animator>();

    std::string boneName(node->mName.C_Str());

    std::unique_ptr<BoneNode> bRoot = std::make_unique<BoneNode>("RootBone", -1);

    size_t index = 0;

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        CreateBoneNode(node->mChildren[i], index, bRoot.get(), nullptr, nameToIndex, nameToOffset);
    }

    animator->SetBoneTree(std::move(bRoot), nameToIndex.size());

    for (size_t i = 0; i < scene->mNumAnimations; i++)
    {
        std::unique_ptr<AnimationClip> aniClip = std::make_unique<AnimationClip>();
    }

    return animator;
}

void CreateAnimationClip()
{
}

void CreateBoneNode(aiNode* node, size_t& curIndex, BoneNode* root, BoneNode* parent,
                                        std::unordered_map<std::string, UINT>& nameToIndex, std::unordered_map<std::string, XMMATRIX>& nameTomOffset)
{
    std::string name(node->mName.C_Str());
    auto it = nameToIndex.find(name);
    bool isBone = it != nameToIndex.end();

    BoneNode* parentBone = parent;

    if (isBone)
    {
        std::unique_ptr<BoneNode> bone = std::make_unique<BoneNode>(std::string(node->mName.C_Str()), curIndex);
        nameToIndex[name] = curIndex;
        curIndex++;

        XMMATRIX bindLocal = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation.Transpose()));
        bone->SetBindLocal(bindLocal);

        bool check = nameTomOffset.find(name) != nameTomOffset.end();
        bone->SetBoneOffset(check ? nameTomOffset[name] : XMMatrixIdentity());

        bool parentIsBone = node->mParent &&
            (nameToIndex.find(node->mParent->mName.C_Str()) != nameToIndex.end());

        BoneNode* raw = bone.get();

        if (parentIsBone && parent)
        {
            parent->Attach(std::move(bone));
        }
        else
        {
            root->Attach(std::move(bone));
        }

        parentBone = raw;
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        CreateBoneNode(node->mChildren[i], curIndex, root, parentBone, nameToIndex, nameTomOffset);
    }
}

std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, XMMATRIX& parentTM, const std::wstring& filepath)
{
    std::wstring name(Utf8ToWString(node->mName.C_Str()));

    if (name == L"Camera" || name == L"Light")
        return nullptr;

    auto meshnode = std::make_unique<MeshNode>();

    meshnode->m_name = name;
    
    aiVector3D scale;
    aiVector3D rot;
    aiVector3D pos;

    aiMatrix4x4 aiMat = node->mTransformation.Transpose();

    XMMATRIX current = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&aiMat)) * parentTM;
    
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshkey, matkey] = CreateMesh(aiMesh, scene, nodeNum, filepath);

        auto model = std::make_unique<Mesh>();
        model->Create(meshkey, matkey, current);

        meshnode->m_Meshs.push_back(std::move(model));
    }

    int num = 0;

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        auto child = CreateNode(node->mChildren[i], scene, num, current, filepath); //자식 노드 탐색
        if(!child)
            continue;
        meshnode->m_Childs.push_back(std::move(child));
        num++;
    }

    return meshnode;
}

std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::wstring& filepath)
{
    std::vector<VERTEX_Pos> vtxPos;
    std::vector<VERTEX_Nrm> vtxNrm;
    std::vector<VERTEX_UV> vtxUV;
    std::vector<VERTEX_T> vtxTan;
    std::vector<VERTEX_B> vtxBi;
    std::vector<INDEX> indices;

    VertexStreams vs;
    MaterialDesc md{};

    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        VERTEX_Pos vPos;
        /*vPos.x = aiMesh->mVertices[i].x;
        vPos.y = -aiMesh->mVertices[i].z;
        vPos.z = aiMesh->mVertices[i].y;*/
        vPos.x = aiMesh->mVertices[i].x;
        vPos.y = aiMesh->mVertices[i].y;
        vPos.z = aiMesh->mVertices[i].z;
        vtxPos.push_back(vPos);
        
        vs.flags = VSF_Pos;
        md.passKey.vertexFlags = VSF_Pos;

        if (aiMesh->HasNormals())
        {
            VERTEX_Nrm vNrm;
            vNrm.nx = aiMesh->mNormals[i].x;
            vNrm.ny = aiMesh->mNormals[i].y;
            vNrm.nz = aiMesh->mNormals[i].z;
            vtxNrm.push_back(vNrm);

            vs.flags |= VSF_Nrm;
            md.passKey.vertexFlags |= VSF_Nrm;
        }

        {
            VERTEX_UV vUV;
            if (aiMesh->HasTextureCoords(0)) //0이 제일 기본 uv
            {
                vUV.u = aiMesh->mTextureCoords[0][i].x;
                vUV.v = aiMesh->mTextureCoords[0][i].y;

                vs.flags |= VSF_UV;
                md.passKey.vertexFlags |= VSF_UV;
            }
            else
            {
                vUV = { 0.0f, 0.0f };
            }
            vtxUV.push_back(vUV);
        }
        
        if (aiMesh->HasTangentsAndBitangents())
        {
            VERTEX_T vtxT;
            VERTEX_B vtxB;

            vtxT.tx = aiMesh->mTangents[i].x;
            vtxT.ty = aiMesh->mTangents[i].y;
            vtxT.tz = aiMesh->mTangents[i].z;

            vtxB.bx = aiMesh->mBitangents[i].x;
            vtxB.by = aiMesh->mBitangents[i].y;
            vtxB.bz = aiMesh->mBitangents[i].z;

            vtxTan.push_back(vtxT);
            vtxBi.push_back(vtxB);

            vs.flags |= VSF_T | VSF_B;
            md.passKey.vertexFlags |= VSF_T | VSF_B;
        }
    }

    for (size_t i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        INDEX idx;

        //파싱 플래그로 무조건 삼각형 face
        idx.V0 = face.mIndices[0]; 
        idx.V1 = face.mIndices[1]; 
        idx.V2 = face.mIndices[2];

        indices.push_back(idx);
    }

    
    vs.vtx_count = aiMesh->mNumVertices;
    vs.pos = vtxPos.data();
    vs.nrm = vtxNrm.data();
    vs.uv = vtxUV.data();
    vs.t = vtxTan.data();
    vs.b = vtxBi.data();

    const auto& renderer = YunoEngine::GetRenderer();

    auto meshHandle = renderer->CreateMesh(vs, indices.data(), aiMesh->mNumFaces);

    aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];

    if (aiMaterial)
    {
        aiString texPath;
        aiReturn ret;
        
        //AlbedoMap
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle diff = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.albedo = diff;
        }
        else
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_Albedo" + std::to_wstring(nodeNum) + L".png";

            TextureHandle diff = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.albedo = diff;
        }
        //NormalMap
        if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle nrm = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.normal = nrm;
        }
        else 
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_Normal" + std::to_wstring(nodeNum) + L".png";

            TextureHandle nrm = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.normal = nrm;
        }
        //Metallic
        if (aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle metal = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.metal = metal;
        }
        else
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_Metallic" + std::to_wstring(nodeNum) + L".png";

            TextureHandle metal = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.metal = metal;
        }
        //Roughness
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle rough = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.rough = rough;
        }
        else
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_Roughness" + std::to_wstring(nodeNum) + L".png";

            TextureHandle rough = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.rough = rough;
        }
        //AO
        if (aiMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle ao = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.ao = ao;
        }
        else 
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_AO" + std::to_wstring(nodeNum) + L".png";

            TextureHandle ao = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.ao = ao;
        }
    }

    auto materialHandle = renderer->CreateMaterial(md);

    return std::make_pair(meshHandle, materialHandle);
}
