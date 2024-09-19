
#include "ModelLoader.h"

Res::ResModel *ModelLoader::Load(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_PreTransformVertices|aiProcess_CalcTangentSpace);
    if(!scene||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
    {
        std::cout<<"Assimp error: "<<importer.GetErrorString()<<"\n";
    }
    auto name = std::filesystem::path(path).stem().string();
    Res::ResModel* model = new Res::ResModel;
    model->path = path;
    ProcessNode(model,scene->mRootNode,scene);
    return model;

}

void ModelLoader::ProcessNode(Res::ResModel *modelNeedLoad, aiNode *node, const aiScene *scene)
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

Res::ResMesh ModelLoader::ProcessMesh(Res::ResModel *modelNeedLoad, aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Res::ResTexture*> textures;

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

    ProcessMaterial(mesh,scene);

    Res::ResMesh meshToVector(vertices, indices);
    return meshToVector;
}

Res::ResMaterial ModelLoader::ProcessMaterial(aiMesh *mesh,const aiScene* scene)
{
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {
        aiString texturePath;
        aiTextureType aiType = (aiTextureType)i;
        int count = material->GetTextureCount(aiType);
        if(count > 0 && material->GetTexture(aiType, 0, &texturePath) == AI_SUCCESS)
        {
            std::cout<<texturePath.C_Str()<<"\n";

            std::filesystem::path absPath = std::filesystem::absolute(texturePath.C_Str());
            std::cout<<absPath<<"\n";

        }
    }

    return Res::ResMaterial();
}
