
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

#include "filesystem"

class ModelLoader
{
public:
    static Res::ResModel* Load(std::string path);
private:
    static void ProcessNode(Res::ResModel* modelNeedLoad,aiNode* node,const aiScene* scene);
    static Res::ResMesh ProcessMesh(Res::ResModel* modelNeedLoad, aiMesh* mesh, const aiScene* scene);
    static Res::ResMaterial ProcessMaterial(aiMesh* mesh,const aiScene* scene);
    static void LoadMaterialTexture(aiMaterial *mat, aiTextureType type,std::string typeName);
    static std::string AnalyzePath(std::string filePath,std::string modelInputPath);
};


#endif //INSTRENDERER_MODELLOADER_H
