#include "pch.h"
#include "YunoRenderer.h"
#include "IMesh.h"
#include "YunoEngine.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Parser.h"

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

bool Parser::LoadFile(const std::string& filename)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || !scene->mRootNode)
        return false;

    CreateNode(scene->mRootNode, scene);

    return true;
}

void Parser::CreateNode(aiNode* node, const aiScene* scene)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        auto mesh_matKey = CreateMesh(aiMesh, scene);
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        CreateNode(node->mChildren[i], scene); //자식 노드 탐색
    }
}

std::pair<MeshHandle, MaterialHandle> Parser::CreateMesh(aiMesh* aiMesh, const aiScene* scene)
{
    std::vector<VERTEX_Pos> vtxPos;
    std::vector<VERTEX_Nrm> vtxNrm;
    std::vector<VERTEX_UV> vtxUV;
    std::vector<VERTEX_T> vtxTan;
    std::vector<VERTEX_B> vtxBi;
    std::vector<INDEX> indices;

    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        VERTEX_Pos vPos;
        vPos.x = aiMesh->mVertices[i].x;
        vPos.y = aiMesh->mVertices[i].y;
        vPos.z = aiMesh->mVertices[i].z;
        vtxPos.push_back(vPos);

        if (aiMesh->HasNormals())
        {
            VERTEX_Nrm vNrm;
            vNrm.nx = aiMesh->mNormals[i].x;
            vNrm.ny = aiMesh->mNormals[i].y;
            vNrm.nz = aiMesh->mNormals[i].z;
            vtxNrm.push_back(vNrm);
        }

        {
            VERTEX_UV vUV;
            if (aiMesh->HasTextureCoords(0)) //0이 제일 기본 uv
            {
                vUV.u = aiMesh->mTextureCoords[0][i].x;
                vUV.v = aiMesh->mTextureCoords[0][i].y;
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

    VertexStreams vs;
    vs.vtx_count = aiMesh->mNumVertices;
    vs.pos = vtxPos.data();
    vs.nrm = vtxNrm.data();
    vs.uv = vtxUV.data();
    vs.t = vtxTan.data();
    vs.b = vtxBi.data();

    const auto& renderer = YunoEngine::GetRenderer();

    auto meshHandle = renderer->CreateMesh(vs, indices.data(), aiMesh->mNumFaces);

    aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];

    MaterialDesc md;

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
