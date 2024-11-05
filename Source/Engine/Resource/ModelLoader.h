
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
    static Res::ResModel* Load(std::string path);
private:
    static void ProcessNode(Res::ResModel* modelNeedLoad,tinygltf::Node* node,tinygltf::Model *model);
    static Res::ResMesh ProcessMesh(Res::ResModel* modelNeedLoad, tinygltf::Mesh* mesh, const tinygltf::Scene* scene);
    static Res::ResMaterial* ProcessMaterial( tinygltf::Mesh* mesh,const tinygltf::Scene* scene,std::string path);
    //static void LoadMaterialTexture(tinygltf::Material *mat, aiTextureType type,std::string typeName);
    static std::string AnalyzePath(std::string filePath,std::string modelInputPath);
    static TextureType GetType(aiTextureType aiType);

    static Res::ResMesh* LoadMesh(tinygltf::Model* model,tinygltf::Mesh& mesh,tinygltf::Primitive& primitive);

    template<typename T>
    static void ReadVertAtt(tinygltf::Model* model,tinygltf::Accessor& accessor,std::vector<T>& outputData);

    static void ReadIndex(tinygltf::Model *model,  tinygltf::Accessor &accessor, std::vector<uint32_t> &outputData);

};


#endif //INSTRENDERER_MODELLOADER_H
