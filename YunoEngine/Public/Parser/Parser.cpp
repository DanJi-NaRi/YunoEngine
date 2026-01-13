<<<<<<< HEAD
﻿#include "pch.h"
=======
#include "pch.h"
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
#include "YunoRenderer.h"
#include "IMesh.h"
#include "YunoEngine.h"
#include "Mesh.h"

#include "Parser.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, const std::string& filepath);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath);
=======
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, const std::string& filepath);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, const std::string& filepath);
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, const std::string& filepath);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath);
>>>>>>> 2e8bd5d (좌표계 수정)
=======
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, const std::string& filepath);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath);
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD:YunoEngine/Public/Parser/Parser.cpp
std::unique_ptr<MeshNode> Parser::LoadFile(const std::string& filepath)
=======
std::unique_ptr<MeshNode> Parser::LoadFile(const std::string& filename)
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋):YunoEngine/Private/Parser/Parser.cpp
=======
std::unique_ptr<MeshNode> Parser::LoadFile(const std::string& filepath)
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
std::unique_ptr<MeshNode> Parser::LoadFile(const std::string& filepath)
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || !scene->mRootNode)
        return {};

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD:YunoEngine/Public/Parser/Parser.cpp
    auto MeshNode = CreateNode(scene->mRootNode, scene, 0, filepath);
=======
    auto MeshNode = CreateNode(scene->mRootNode, scene);
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋):YunoEngine/Private/Parser/Parser.cpp
=======
    auto MeshNode = CreateNode(scene->mRootNode, scene, filepath);
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
    auto MeshNode = CreateNode(scene->mRootNode, scene, 0, filepath);
>>>>>>> 2e8bd5d (좌표계 수정)
=======
    auto MeshNode = CreateNode(scene->mRootNode, scene, 0, filepath);
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

    return MeshNode;
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD:YunoEngine/Public/Parser/Parser.cpp
=======
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, const std::string& filepath)
{
    std::string name(node->mName.C_Str());

    if (name == "Camera" || name == "Light")
        return nullptr;

    auto meshnode = std::make_unique<MeshNode>();

    meshnode->m_name = name;
    
    aiVector3D scale;
    aiVector3D rot;
    aiVector3D pos;

    node->mTransformation.Decompose(scale, rot, pos); //���߿� ���ʹϾ����� �ٲٱ�

    XMFLOAT3 vScale = XMFLOAT3(scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f);
    XMFLOAT3 vRot = XMFLOAT3(rot.x, -rot.z, rot.y);
    XMFLOAT3 vPos = XMFLOAT3(pos.x * 0.01f, -pos.z * 0.01f, pos.y * 0.01f);

    meshnode->pos = vPos;
    meshnode->rot = vRot;
    meshnode->scale = vScale;
    
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshkey, matkey] = CreateMesh(aiMesh, scene, nodeNum, filepath);

        auto model = std::make_unique<Mesh>();
        model->Create(meshkey, matkey, vPos, vRot, vScale);
<<<<<<< HEAD
=======
std::unique_ptr<MeshNode> Parser::CreateNode(aiNode* node, const aiScene* scene)
=======
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, const std::string& filepath)
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, int nodeNum, const std::string& filepath)
>>>>>>> 2e8bd5d (좌표계 수정)
{
    std::string name(node->mName.C_Str());

    if (name == "Camera" || name == "Light")
        return nullptr;

    auto meshnode = std::make_unique<MeshNode>();

    meshnode->m_name = name;
    
    aiVector3D scale;
    aiVector3D rot;
    aiVector3D pos;

    node->mTransformation.Decompose(scale, rot, pos); //���߿� ���ʹϾ����� �ٲٱ�

    XMFLOAT3 vScale = XMFLOAT3(scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f);
    XMFLOAT3 vRot = XMFLOAT3(rot.x, -rot.z, rot.y);
    XMFLOAT3 vPos = XMFLOAT3(pos.x * 0.01f, -pos.z * 0.01f, pos.y * 0.01f);

    meshnode->pos = vPos;
    meshnode->rot = vRot;
    meshnode->scale = vScale;
    
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshkey, matkey] = CreateMesh(aiMesh, scene, nodeNum, filepath);

        auto model = std::make_unique<Mesh>();
<<<<<<< HEAD
        model->Create(meshkey, matkey);
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋):YunoEngine/Private/Parser/Parser.cpp
=======
        model->Create(meshkey, matkey, vPos, vRot, vScale);
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

        meshnode->m_Meshs.push_back(std::move(model));
    }

    int num = 0;

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD:YunoEngine/Public/Parser/Parser.cpp
<<<<<<< HEAD
<<<<<<< HEAD:YunoEngine/Private/Parser/Parser.cpp
        auto child = CreateNode(node->mChildren[i], scene); //�ڽ� ���� Ž��
=======
        auto child = CreateNode(node->mChildren[i], scene, filepath); //�ڽ� ��� Ž��
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌):YunoEngine/Public/Parser/Parser.cpp
=======
        auto child = CreateNode(node->mChildren[i], scene, num, filepath); //�ڽ� ��� Ž��
        if(!child)
            continue;
>>>>>>> 7165d64 (좌표계 수정)
        meshnode->m_Childs.push_back(std::move(child));
        num++;
=======
        auto child = CreateNode(node->mChildren[i], scene); //�ڽ� ��� Ž��
=======
        auto child = CreateNode(node->mChildren[i], scene, filepath); //�ڽ� ��� Ž��
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
        meshnode->m_Childs.push_back(std::move(child));
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋):YunoEngine/Private/Parser/Parser.cpp
=======
=======
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
        auto child = CreateNode(node->mChildren[i], scene, num, filepath); //�ڽ� ��� Ž��
        if(!child)
            continue;
        meshnode->m_Childs.push_back(std::move(child));
        num++;
<<<<<<< HEAD
>>>>>>> 2e8bd5d (좌표계 수정)
=======
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
    }

    return meshnode;
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath)
=======
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, const std::string& filepath)
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath)
>>>>>>> 2e8bd5d (좌표계 수정)
=======
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, int nodeNum, const std::string& filepath)
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
{
    std::vector<VERTEX_Pos> vtxPos;
    std::vector<VERTEX_Nrm> vtxNrm;
    std::vector<VERTEX_UV> vtxUV;
    std::vector<VERTEX_T> vtxTan;
    std::vector<VERTEX_B> vtxBi;
    std::vector<INDEX> indices;

    VertexStreams vs;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    MaterialDesc md{};
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
    MaterialDesc md{};
>>>>>>> 2e8bd5d (좌표계 수정)
=======
    MaterialDesc md{};
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        VERTEX_Pos vPos;
        vPos.x = aiMesh->mVertices[i].x;
        vPos.y = -aiMesh->mVertices[i].z;
        vPos.z = aiMesh->mVertices[i].y;
        vtxPos.push_back(vPos);
        
        vs.flags = VSF_Pos;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        md.passKey.vertexFlags = VSF_Pos;
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
        md.passKey.vertexFlags = VSF_Pos;
>>>>>>> 2e8bd5d (좌표계 수정)
=======
        md.passKey.vertexFlags = VSF_Pos;
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

        if (aiMesh->HasNormals())
        {
            VERTEX_Nrm vNrm;
            vNrm.nx = aiMesh->mNormals[i].x;
            vNrm.ny = -aiMesh->mNormals[i].z;
            vNrm.nz = aiMesh->mNormals[i].y;
            vtxNrm.push_back(vNrm);

            vs.flags |= VSF_Nrm;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            md.passKey.vertexFlags |= VSF_Nrm;
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
            md.passKey.vertexFlags |= VSF_Nrm;
>>>>>>> 2e8bd5d (좌표계 수정)
=======
            md.passKey.vertexFlags |= VSF_Nrm;
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
        }

        {
            VERTEX_UV vUV;
<<<<<<< HEAD
            if (aiMesh->HasTextureCoords(0)) //0이 제일 기본 uv
=======
            if (aiMesh->HasTextureCoords(0)) //0�� ���� �⺻ uv
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
            {
                vUV.u = aiMesh->mTextureCoords[0][i].x;
                vUV.v = aiMesh->mTextureCoords[0][i].y;

                vs.flags |= VSF_UV;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
                md.passKey.vertexFlags |= VSF_UV;
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
                md.passKey.vertexFlags |= VSF_UV;
>>>>>>> 2e8bd5d (좌표계 수정)
=======
                md.passKey.vertexFlags |= VSF_UV;
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
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
            vtxT.ty = -aiMesh->mTangents[i].z;
            vtxT.tz = aiMesh->mTangents[i].y;

            vtxB.bx = aiMesh->mBitangents[i].x;
            vtxB.by = -aiMesh->mBitangents[i].z;
            vtxB.bz = aiMesh->mBitangents[i].y;

            vtxTan.push_back(vtxT);
            vtxBi.push_back(vtxB);

            vs.flags |= VSF_T | VSF_B;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            md.passKey.vertexFlags |= VSF_T | VSF_B;
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
            md.passKey.vertexFlags |= VSF_T | VSF_B;
>>>>>>> 2e8bd5d (좌표계 수정)
=======
            md.passKey.vertexFlags |= VSF_T | VSF_B;
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
        }
    }

    for (size_t i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        INDEX idx;

<<<<<<< HEAD
        //파싱 플래그로 무조건 삼각형 face
=======
        //�Ľ� �÷��׷� ������ �ﰢ�� face
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
    MaterialDesc md{};

>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 2e8bd5d (좌표계 수정)
=======
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6
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
            auto texPath = filepath.substr(0, filepath.find(".fbx"));
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            texPath += "_Albedo" + std::to_string(nodeNum) + ".png";
=======
            texPath += "_Albedo" + std::to_string(aiMesh->mMaterialIndex) + ".png";
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
            texPath += "_Albedo" + std::to_string(nodeNum) + ".png";
>>>>>>> 2e8bd5d (좌표계 수정)
=======
            texPath += "_Albedo" + std::to_string(nodeNum) + ".png";
>>>>>>> 6ded2033580cf12334606aed8b92a20d427da9e6

            auto wPath = Utf8ToWString(texPath.c_str());
            TextureHandle diff = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.albedo = diff;
        }
        //NormalMap
        if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle nrm = renderer->CreateTexture2DFromFile(wPath.c_str());

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
            ret = aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, md.metallic);
            if (ret == AI_FAILURE)
                md.metallic = 0.0f;
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
            ret = aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, md.roughness);
            if (ret == AI_FAILURE)
                md.roughness = 0.5f;
        }
        //AO
        if (aiMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texPath) == AI_SUCCESS)
        {
            auto wPath = Utf8ToWString(texPath.C_Str());
            TextureHandle ao = renderer->CreateTexture2DFromFile(wPath.c_str());

            md.ao = ao;
        }
    }

    auto materialHandle = renderer->CreateMaterial(md);

    return std::make_pair(meshHandle, materialHandle);
}
