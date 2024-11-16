
#include "ModelLoader.h"
#include "ResourceManager.h"
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

/*Res::ResModel *ModelLoader::Load(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_PreTransformVertices|aiProcess_CalcTangentSpace);
    if(!scene||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
    {
        std::cout<<"Assimp error: "<<importer.GetErrorString()<<"\n";
    }
    auto name = std::filesystem::path(path).stem().string();
    Res::ResModel* model = new Res::ResModel(path);
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
    std::string name;

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

    auto material = ProcessMaterial(mesh,scene,modelNeedLoad->path);

    Res::ResMesh meshToVector(vertices, indices,mesh->mName.C_Str(),material);
    return meshToVector;
}

Res::ResMaterial* ModelLoader::ProcessMaterial(aiMesh *mesh,const aiScene* scene,std::string path)
{
    ResMaterial* resMaterial = new ResMaterial;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {

        aiString texturePath;
        aiTextureType aiType = (aiTextureType)i;
        int count = material->GetTextureCount(aiType);
        
        if(count > 0 && material->GetTexture(aiType, 0, &texturePath) == AI_SUCCESS)
        {
            std::string finPath =GetPathWithoutName(path)+texturePath.C_Str();
            std::cout<<finPath<<"\n";
            *//*auto name = ResourceManager::Get().LoadResource(finPath);
            auto texture = (Res::ResTexture*)ResourceManager::Get().resReg[name];
            texture->textureType = GetType(aiType);
            resMaterial->AddTexture(texture);*//*
            //std::cout<<aiType<<" ";
            //std::filesystem::path absPath = std::filesystem::absolute(texturePath.C_Str());
            //std::cout<<absPath<<"\n";
        }
    }
    return resMaterial;
}

TextureType ModelLoader::GetType(aiTextureType aiType)
{
    switch (aiType)
    {
        case aiTextureType_EMISSIVE:
            return TextureType::Emission;
        case aiTextureType_NORMALS:
            return TextureType::Normal;
        case aiTextureType_BASE_COLOR:
            return TextureType::BaseColor;
        case aiTextureType_METALNESS:
            return TextureType::Metallic;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            return TextureType::Roughness;
        case aiTextureType_AMBIENT_OCCLUSION:
            return TextureType::Ao;
        default:
            return TextureType::DontCare;
    }

}*/
Res::ResModel *ModelLoader::Load(std::string path)
{
    Res::ResModel* resModel = new ResModel(path);

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }
    if (!ret) {
        printf("Failed to parse glTF\n");
    }

    tinygltf::Scene scene = model.scenes[model.defaultScene];
    for (auto& index:scene.nodes)
    {
        auto node = model.nodes[index];
        ProcessNode(resModel,&node,&model);
    }

    return resModel;
}

void ModelLoader::ProcessNode(Res::ResModel *modelNeedLoad, tinygltf::Node *node, tinygltf::Model *model)
{
    if(node->mesh!=-1)
    {
        auto& gltfMesh = model->meshes[node->mesh];
        for(int j =0;j<gltfMesh.primitives.size();++j)
        {
            auto& pri = model->meshes[node->mesh].primitives[j];
            Res::ResMesh* mesh = LoadMesh(model,gltfMesh,pri);
            modelNeedLoad->meshes.push_back(mesh);
        }
    }


    for (int i = 0; i < node->children.size(); ++i)
    {
        ProcessNode(modelNeedLoad,&model->nodes[node->children[i]],model);
    }
}

/*Res::ResMesh ModelLoader::ProcessMesh(Res::ResModel *modelNeedLoad, tinygltf::Mesh *mesh, const tinygltf::Scene *scene)
{
    return Res::ResMesh(std::vector(), std::vector(), __cxx11::basic_string());
}*/

Res::ResMaterial *ModelLoader::ProcessMaterial(tinygltf::Mesh *mesh, const tinygltf::Scene *scene, std::string path)
{
    return nullptr;
}

Res::ResMesh* ModelLoader::LoadMesh(tinygltf::Model* model,tinygltf::Mesh& mesh, tinygltf::Primitive& primitive)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;

    //Vertex vert;
    if(primitive.attributes.find("POSITION")!=primitive.attributes.end())
    {
        tinygltf::Accessor &accessor = model->accessors[primitive.attributes.at("POSITION")];

        ReadVertAtt(model,accessor,positions);
    }
    if(primitive.attributes.find("NORMAL")!=primitive.attributes.end())
    {
        tinygltf::Accessor &accessor = model->accessors[primitive.attributes.at("NORMAL")];

        ReadVertAtt(model,accessor,normals);
    }
    if(primitive.attributes.find("TEXCOORD_0")!=primitive.attributes.end())
    {
        tinygltf::Accessor &accessor = model->accessors[primitive.attributes.at("TEXCOORD_0")];

        ReadVertAtt(model,accessor,uvs);
    }
    if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
        tinygltf::Accessor &accessor = model->accessors[primitive.attributes.at("TANGENT")];
        ReadVertAtt(model, accessor, tangents);
    }

    //Index
    {
        tinygltf::Accessor &accessor = model->accessors[primitive.indices];
        ReadIndex(model,accessor,indices);
    }
    //Copy
    vertices.resize(positions.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position = positions[i];
        vertices[i].normal = i < normals.size() ? normals[i] : glm::vec3(0.0f);
        vertices[i].uv = i < uvs.size() ? uvs[i] : glm::vec2(0.0f);
        vertices[i].tangent = i < tangents.size() ? tangents[i] : glm::vec4(0.0f);
        vertices[i].bitangent = glm::cross(vertices[i].normal, glm::vec3(vertices[i].tangent)) * vertices[i].tangent.w;
    }

    //Material
    ResMaterial* resMaterial = new ResMaterial();

    {
        int materialIndex = primitive.material;
        if(primitive.material>=0&&materialIndex<model->materials.size())
        {
            tinygltf::Material& material = model->materials[materialIndex];
            std::string name = material.name;

            auto& pbr = material.pbrMetallicRoughness;

            //Albedo Texture
            int albedoTextureIndex = pbr.baseColorTexture.index;


            if(albedoTextureIndex >= 0)
            {
                auto& albedoTexture = model->textures[albedoTextureIndex];
                int albedoImageIndex = albedoTexture.source;

                const tinygltf::Image &image = model->images[albedoImageIndex];
                int width = image.width;
                int height = image.height;
                auto imageData = image.image;
                size_t imageSize = image.image.size();

                //TODO ResManager to create
                Res::ResTexture* resTexture = new Res::ResTexture(width,height,imageSize,imageData,image.uri);
                resTexture->textureType = TextureType::BaseColor;
                resMaterial->AddTexture(resTexture);
            }

            //Normal Texture
            int normalTextureIndex = material.normalTexture.index;


            if(normalTextureIndex >= 0)
            {
                auto& normalTexture = model->textures[normalTextureIndex];
                int normalImageIndex = normalTexture.source;
                const tinygltf::Image &image = model->images[normalImageIndex];
                int width = image.width;
                int height = image.height;
                auto imageData = image.image;
                size_t imageSize = image.image.size();


                //TODO ResManager to create
                Res::ResTexture* resTexture = new Res::ResTexture(width,height,imageSize,imageData,image.uri);
                resTexture->textureType = TextureType::Normal;
                resMaterial->AddTexture(resTexture);
            }

            //M&R Texture
            int mrTextureIndex = pbr.metallicRoughnessTexture.index;


            if(mrTextureIndex >= 0)
            {
                auto& mrTexture = model->textures[mrTextureIndex];
                int mrImageIndex = mrTexture.source;

                const tinygltf::Image &image = model->images[mrImageIndex];
                int width = image.width;
                int height = image.height;
                auto imageData = image.image;
                size_t imageSize = image.image.size();

                //TODO ResManager to create
                Res::ResTexture* resTexture = new Res::ResTexture(width,height,imageSize,imageData,image.uri);
                resTexture->textureType = TextureType::RoughnessMetallic;
                resMaterial->AddTexture(resTexture);
            }

        }
    }


    return new Res::ResMesh(vertices,indices,mesh.name,resMaterial);
}


void ModelLoader::ReadIndex(tinygltf::Model *model,  tinygltf::Accessor &accessor, std::vector<uint32_t> &outputData)
{
    const tinygltf::BufferView &bufferView = model->bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];

    // 确定索引的元素大小
    size_t indexCount = accessor.count;
    size_t indexSize = accessor.ByteStride(bufferView) > 0 ? accessor.ByteStride(bufferView) : sizeof(uint32_t); // 默认使用 uint32_t

    // 计算索引数据的起始位置
    const uint8_t *indexBuffer = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

    // Resize outputData to hold the data
    outputData.resize(indexCount);

    // 读取索引数据
    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
        memcpy(outputData.data(), indexBuffer, indexCount * indexSize);
    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        std::vector<uint16_t> tempIndices(indexCount);
        memcpy(tempIndices.data(), indexBuffer, indexCount * indexSize);
        for (size_t i = 0; i < indexCount; ++i) {
            outputData[i] = static_cast<uint32_t>(tempIndices[i]);
        }
    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        std::vector<uint8_t> tempIndices(indexCount);
        memcpy(tempIndices.data(), indexBuffer, indexCount * indexSize);
        for (size_t i = 0; i < indexCount; ++i) {
            outputData[i] = static_cast<uint32_t>(tempIndices[i]);
        }
    } else {
        std::cerr << "Unsupported index component type: " << accessor.componentType << std::endl;
    }
}

template<typename T>
void ModelLoader::ReadVertAtt(tinygltf::Model *model, tinygltf::Accessor &accessor, std::vector<T>& outputData)
{
    const tinygltf::BufferView &bufferView = model->bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];
    const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

    outputData.resize(accessor.count);
    std::memcpy(outputData.data(),dataPtr,accessor.count*sizeof(T));
}

