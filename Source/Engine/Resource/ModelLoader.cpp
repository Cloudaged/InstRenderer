
#include "ModelLoader.h"

Res::Model *ModelLoader::Load(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_PreTransformVertices|aiProcess_CalcTangentSpace);
    if(!scene||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
    {
        std::cout<<"Assimp error: "<<importer.GetErrorString()<<"\n";
    }
    auto name = std::filesystem::path(path).stem().string();
    Res::Model* model = new Res::Model;
    model->path = path;
    ProcessNode(model,scene->mRootNode,scene);
    return model;

}

void ModelLoader::ProcessNode(Res::Model *modelNeedLoad, aiNode *node, const aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        modelNeedLoad->meshes.push_back(ProcessMesh(modelNeedLoad,mesh,scene));
    }

    for (int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(modelNeedLoad,node->mChildren[i],scene);
    }
}

Res::ResMesh ModelLoader::ProcessMesh(Res::Model *modelNeedLoad, aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Res::Texture*> textures;

    //Vertex Buffer
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangent = vector;

        if(mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    //Index
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    Res::ResMesh meshToVector(vertices, indices);
    return meshToVector;
}
