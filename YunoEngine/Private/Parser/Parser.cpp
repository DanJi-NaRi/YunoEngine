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


std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, std::unordered_map<std::string, UINT>& nameToIndex, XMMATRIX& parentTM, const std::wstring& filepath);
std::unique_ptr<Animator> CreateAnimator(aiNode* node, const aiScene* scene, std::unordered_map<std::string, UINT>& boneNameSet, std::unordered_map<std::string, XMMATRIX>& nameToOffset);
void CreateAnimationClip(aiAnimation* anim, const aiScene* scene, std::unordered_map<std::string, UINT>& nameToIndex, AnimationClip* out);
void CreateBoneNameSet(const aiScene* scene, std::unordered_map<std::string, UINT>& indexOut, std::unordered_map<std::string, XMMATRIX>& matrixOut);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, std::unordered_map<std::string, UINT>& nameToIndex, const std::wstring& filepath);
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

    std::unique_ptr<Animator> animator = nullptr;

    if (scene->HasAnimations())
        if (CheckSkeletalModel(scene))
        {
            CreateBoneNameSet(scene, BoneNameToIndex, BoneNameToOffset);
            animator = CreateAnimator(scene->mRootNode, scene, BoneNameToIndex, BoneNameToOffset);
        }

    XMMATRIX root = XMMatrixIdentity();

    auto MeshNode = CreateNode(scene->mRootNode, scene, 0, BoneNameToIndex, root, filepath);

    return std::make_pair(std::move(MeshNode), std::move(animator));
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
        aiAnimation* aiAnim = scene->mAnimations[i];

        std::unique_ptr<AnimationClip> clip = std::make_unique<AnimationClip>();
        clip->duration = (UINT)aiAnim->mDuration;
        clip->TickPerSec =
            aiAnim->mTicksPerSecond != 0
            ? (UINT)aiAnim->mTicksPerSecond
            : 25.0f;

        clip->channels.resize(nameToIndex.size());

        CreateAnimationClip(aiAnim, scene, nameToIndex, clip.get());
        //임시임시임시
        clip->isLoop = true;

        animator->AddAnimationClip(std::to_string(i), std::move(clip));
    }

    return animator;
}

void CreateAnimationClip(aiAnimation* anim, const aiScene* scene, std::unordered_map<std::string, UINT>& nameToIndex, AnimationClip* out)
{
    for (size_t i = 0; i < anim->mNumChannels; i++)
    {
        auto channel = anim->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        auto it = nameToIndex.find(boneName);
        if(it == nameToIndex.end()) continue;

        std::unique_ptr<AnimationClips> boneClip = std::make_unique<AnimationClips>();
        boneClip->boneName = boneName;

        for (size_t j = 0; j < channel->mNumPositionKeys; j++)
        {
            TransKey transkey;
            transkey.TickTime = channel->mPositionKeys[j].mTime;
            auto posKey = channel->mPositionKeys[j].mValue;
            transkey.trans = XMFLOAT3(posKey.x, posKey.y, posKey.z);
            boneClip->TransKeys.push_back(transkey);
        }

        for (size_t j = 0; j < channel->mNumRotationKeys; j++)
        {
            QuatKey quatkey;
            quatkey.TickTime = channel->mRotationKeys[j].mTime;
            auto rotKey = channel->mRotationKeys[j].mValue;
            quatkey.quat = XMFLOAT4(rotKey.x, rotKey.y, rotKey.z, rotKey.w);
            boneClip->QuatKeys.push_back(quatkey);
        }

        for (size_t j = 0; j < channel->mNumScalingKeys; j++)
        {
            ScaleKey scalekey;
            scalekey.TickTime = channel->mScalingKeys[j].mTime;
            auto scalingkey = channel->mScalingKeys[j].mValue;
            scalekey.scale = XMFLOAT3(scalingkey.x, scalingkey.y, scalingkey.z);
            boneClip->ScaleKeys.push_back(scalekey);
        }

        UINT index = it->second;
        out->channels[index] = std::make_unique<BoneAnimation>(std::move(boneClip));
    }
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

std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, std::unordered_map<std::string, UINT>& nameToIndex, XMMATRIX& parentTM, const std::wstring& filepath)
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

    XMMATRIX current = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&aiMat));
    meshnode->mUserTM = current;
    
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshkey, matkey] = CreateMesh(aiMesh, scene, nodeNum, nameToIndex, filepath);

        auto model = std::make_unique<Mesh>();
        model->Create(meshkey, matkey);

        meshnode->m_Meshs.push_back(std::move(model));
    }

    int num = 0;

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        auto child = CreateNode(node->mChildren[i], scene, num, nameToIndex, current, filepath); //자식 노드 탐색
        if(!child)
            continue;
        meshnode->m_Childs.push_back(std::move(child));
        num++;
    }

    return meshnode;
}

std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, 
                                            std::unordered_map<std::string, UINT>& nameToIndex, const std::wstring& filepath)
{
    std::vector<VERTEX_Pos> vtxPos;
    std::vector<VERTEX_Nrm> vtxNrm;
    std::vector<VERTEX_UV> vtxUV;
    std::vector<VERTEX_T> vtxTan;
    std::vector<VERTEX_B> vtxBi;
    std::vector<VERTEX_BoneIndex> vtxBoneindex;
    std::vector<VERTEX_BoneWeight> vtxBoneweight;
    std::vector<INDEX> indices;

    VertexStreams vs;
    MaterialDesc md{};

    std::vector<std::vector<VertexWeight>> tempWeights(aiMesh->mNumVertices);

    if (aiMesh->HasBones())
    {
        for (UINT b = 0; b < aiMesh->mNumBones; b++)
        {
            aiBone* bone = aiMesh->mBones[b];
            UINT boneIndex = nameToIndex[bone->mName.C_Str()];

            for (UINT w = 0; w < bone->mNumWeights; w++)
            {
                const aiVertexWeight& vw = bone->mWeights[w];
                tempWeights[vw.mVertexId].push_back({
                    boneIndex,
                    vw.mWeight
                    });
            }
        }
    }

    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        VERTEX_Pos vPos;
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

        if (aiMesh->HasBones())
        {
            VERTEX_BoneIndex index;
            VERTEX_BoneWeight weight;

            auto& vw = tempWeights[i];
            std::sort(vw.begin(), vw.end(), [](VertexWeight& a, VertexWeight& b) { return a.weight > b.weight; });

            UINT idx[4] = { 0, 0, 0, 0 };
            float w[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

            if (!vw.empty())
            {
                float sum = 0.0f;
                for (int i = 0; i < 4 && i < vw.size(); i++)
                    sum += vw[i].weight;

                for (size_t j = 0; j < vw.size() && j < 4; j++)
                {
                    idx[j] = vw[j].boneIndex;
                    if (sum > 0.0f)
                        w[j] = vw[j].weight / sum; //정규화
                    else
                        w[j] = 0.0f;
                }
            }
           
            index = { idx[0], idx[1], idx[2], idx[3] };
            weight = { w[0], w[1], w[2], w[3] };

            vtxBoneindex.push_back(index);
            vtxBoneweight.push_back(weight);

            vs.flags |= VSF_BoneIndex | VSF_BoneWeight;
            md.passKey.vertexFlags |= VSF_BoneIndex | VSF_BoneWeight;
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
    vs.boneWeight = vtxBoneweight.data();
    vs.boneIdx = vtxBoneindex.data();

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

            md.metal = 123; //pbr셰이더용 쓰레기값 안넣어주면 터짐 추가 텍스쳐 안넣어주면 터짐
        }
        else
        {
            auto texPath = filepath.substr(0, filepath.find(L".fbx"));
            texPath += L"_Metallic" + std::to_wstring(nodeNum) + L".png";

            TextureHandle metal = renderer->CreateTexture2DFromFile(texPath.c_str());

            md.metal = 1;
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
