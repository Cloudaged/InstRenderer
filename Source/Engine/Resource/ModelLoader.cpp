
#include "ModelLoader.h"
#include "ResourceManager.h"
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"


std::shared_ptr<Res::ResModel> ModelLoader::Load(std::string path)
{
    std::shared_ptr<Res::ResModel> resModel = std::make_shared<Res::ResModel>(path);

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
        resModel->rootNode =  ProcessNode(resModel,std::make_shared<ResNode>(nullptr,scene.name),&node,&model);
    }

    return resModel;
}

std::shared_ptr<Res::ResNode> ModelLoader::ProcessNode(std::shared_ptr<Res::ResModel> modelNeedLoad,std::shared_ptr<ResNode> parent,
                                       tinygltf::Node *node, tinygltf::Model *model)
{
    std::shared_ptr<Res::ResNode> resNode = std::make_shared<ResNode>(parent,node->name);

    if(node->mesh!=-1)
    {
        auto& gltfMesh = model->meshes[node->mesh];
        for(int j =0;j<gltfMesh.primitives.size();++j)
        {
            auto& pri = model->meshes[node->mesh].primitives[j];
            std::shared_ptr<Res::ResMesh> mesh = LoadMesh(modelNeedLoad,model,gltfMesh,pri);
            resNode->meshes.push_back(mesh);
        }
    }


    for (int i = 0; i < node->children.size(); ++i)
    {
        auto child = ProcessNode(modelNeedLoad,resNode,&model->nodes[node->children[i]],model);
        resNode->children.push_back(child);
    }
    return resNode;
}


std::shared_ptr<Res::ResMesh> ModelLoader::LoadMesh(std::shared_ptr<Res::ResModel> resModel,tinygltf::Model* model,tinygltf::Mesh& mesh, tinygltf::Primitive& primitive)
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
        resModel->minPoint = glm::min(resModel->minPoint,positions[i]);
        resModel->maxPoint = glm::max(resModel->maxPoint,positions[i]);
        vertices[i].normal = i < normals.size() ? normals[i] : glm::vec3(0.0f);
        vertices[i].uv = i < uvs.size() ? uvs[i] : glm::vec2(0.0f);
        if(i < tangents.size())
        {
            vertices[i].tangent = tangents[i];
            vertices[i].bitangent = glm::cross(vertices[i].normal, glm::vec3(vertices[i].tangent))*vertices[i].tangent.w;
        } else
        {
            vertices[i].tangent = glm::vec4(0,0,0,0);
            vertices[i].bitangent =glm::cross(vertices[i].normal, glm::vec3(vertices[i].tangent));
        }
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
                std::shared_ptr<Res::ResTexture> resTexture = std::make_shared<Res::ResTexture>(RESTAG(image.name),width,height,imageSize,imageData,image.uri);
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
                std::shared_ptr<Res::ResTexture> resTexture = std::make_shared<Res::ResTexture>(RESTAG(image.name),width,height,imageSize,imageData,image.uri);
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
                std::shared_ptr<Res::ResTexture> resTexture = std::make_shared<Res::ResTexture>(RESTAG(image.name),width,height,imageSize,imageData,image.uri);
                resTexture->textureType = TextureType::RoughnessMetallic;
                resMaterial->AddTexture(resTexture);
            }

        }
    }


    return std::make_shared<Res::ResMesh>(vertices,indices,mesh.name,resMaterial);
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

