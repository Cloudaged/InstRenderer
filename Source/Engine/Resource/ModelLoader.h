
#ifndef INSTRENDERER_MODELLOADER_H
#define INSTRENDERER_MODELLOADER_H
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include "ResModel.h"
#include "ResMesh.h"
#include "iostream"
#include "filesystem"
#include "ResTexture.h"
#include "ResMaterial.h"
#include "../Common/Core/PathDefine.h"

#include "filesystem"
#include "tiny_gltf.h"

class ModelLoader
{
public:
    static std::shared_ptr<Res::ResModel> Load(std::string path);
private:
    static std::shared_ptr<Res::ResNode> ProcessNode(std::shared_ptr<Res::ResModel> modelNeedLoad,std::shared_ptr<Res::ResNode> parent,tinygltf::Node* node,tinygltf::Model *model);
    static std::shared_ptr<Res::ResMesh> LoadMesh(std::shared_ptr<Res::ResModel> resModel,tinygltf::Model* model,tinygltf::Mesh& mesh,tinygltf::Primitive& primitive);
    static void ReadIndex(tinygltf::Model *model,  tinygltf::Accessor &accessor, std::vector<uint32_t> &outputData);
    template<typename T>
    static void ReadVertAtt(tinygltf::Model* model,tinygltf::Accessor& accessor,std::vector<T>& outputData);
};


#endif //INSTRENDERER_MODELLOADER_H
