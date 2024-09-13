
#ifndef INSTRENDERER_MODELLOADER_H
#define INSTRENDERER_MODELLOADER_H
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include "Model.h"
#include "ResMesh.h"
#include "iostream"
#include "filesystem"
#include "Texture.h"
class ModelLoader
{
public:
    static Res::Model* Load(std::string path);
private:
    static void ProcessNode(Res::Model* modelNeedLoad,aiNode* node,const aiScene* scene);
    static Res::ResMesh ProcessMesh(Res::Model* modelNeedLoad, aiMesh* mesh, const aiScene* scene);
    static void LoadMaterialTexture(aiMaterial *mat, aiTextureType type,std::string typeName);
    static std::string AnalyzePath(std::string filePath,std::string modelInputPath);
};


#endif //INSTRENDERER_MODELLOADER_H
