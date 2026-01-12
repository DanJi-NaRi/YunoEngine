#include "pch.h"
#include "YunoRenderer.h"
#include "IMesh.h"
#include "YunoEngine.h"
#include "Mesh.h"

#include "Parser.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, const std::string& filepath);
std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, const std::string& filepath);

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

std::unique_ptr<MeshNode> Parser::LoadFile(const std::string& filepath)
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

    auto MeshNode = CreateNode(scene->mRootNode, scene, filepath);

    return MeshNode;
}

std::unique_ptr<MeshNode> CreateNode(aiNode* node, const aiScene* scene, const std::string& filepath)
{
    auto meshnode = std::make_unique<MeshNode>();

    meshnode->m_name = std::string(node->mName.C_Str());
    
    aiVector3D scale;
    aiVector3D rot;
    aiVector3D pos;
    node->mTransformation.Decompose(scale, rot, pos); //���߿� ���ʹϾ����� �ٲٱ�

    XMFLOAT3 vScale = XMFLOAT3(scale.x, scale.y, scale.z);
    XMFLOAT3 vRot = XMFLOAT3(rot.x, rot.y, rot.z);
    XMFLOAT3 vPos = XMFLOAT3(pos.x, pos.y, pos.z);

    meshnode->pos = vPos;
    meshnode->rot = vRot;
    meshnode->scale = vScale;
    
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshkey, matkey] = CreateMesh(aiMesh, scene, filepath);

        auto model = std::make_unique<Mesh>();
        model->Create(meshkey, matkey, vPos, vRot, vScale);

        meshnode->m_Meshs.push_back(std::move(model));
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
<<<<<<< HEAD:YunoEngine/Private/Parser/Parser.cpp
        auto child = CreateNode(node->mChildren[i], scene); //�ڽ� ���� Ž��
=======
        auto child = CreateNode(node->mChildren[i], scene, filepath); //�ڽ� ��� Ž��
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌):YunoEngine/Public/Parser/Parser.cpp
        meshnode->m_Childs.push_back(std::move(child));
    }

    return meshnode;
}

std::pair<MeshHandle, MaterialHandle> CreateMesh(aiMesh* aiMesh, const aiScene* scene, const std::string& filepath)
{
    std::vector<VERTEX_Pos> vtxPos;
    std::vector<VERTEX_Nrm> vtxNrm;
    std::vector<VERTEX_UV> vtxUV;
    std::vector<VERTEX_T> vtxTan;
    std::vector<VERTEX_B> vtxBi;
    std::vector<INDEX> indices;

    VertexStreams vs;

    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        VERTEX_Pos vPos;
        vPos.x = aiMesh->mVertices[i].x;
        vPos.y = aiMesh->mVertices[i].y;
        vPos.z = aiMesh->mVertices[i].z;
        vtxPos.push_back(vPos);
        
        vs.flags = VSF_Pos;

        if (aiMesh->HasNormals())
        {
            VERTEX_Nrm vNrm;
            vNrm.nx = aiMesh->mNormals[i].x;
            vNrm.ny = aiMesh->mNormals[i].y;
            vNrm.nz = aiMesh->mNormals[i].z;
            vtxNrm.push_back(vNrm);

            vs.flags |= VSF_Nrm;
        }

        {
            VERTEX_UV vUV;
            if (aiMesh->HasTextureCoords(0)) //0이 제일 기본 uv
            {
                vUV.u = aiMesh->mTextureCoords[0][i].x;
                vUV.v = aiMesh->mTextureCoords[0][i].y;

                vs.flags |= VSF_UV;
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

    MaterialDesc md{};

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
            texPath += "_Albedo" + std::to_string(aiMesh->mMaterialIndex) + ".png";

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
