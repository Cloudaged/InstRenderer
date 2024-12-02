
#ifndef INSTRENDERER_RESOURCEMANAGER_H
#define INSTRENDERER_RESOURCEMANAGER_H
#include "map"
#include "ResBase.h"
#include "filesystem"
#include "ModelLoader.h"
#include "memory"
#include "../Common/Core/PathDefine.h"
#include "ImageLoader.h"
#include "../Render/Mesh.h"
#include "../Render/Material/StandardMaterial.h"
#include "thread"
#include "../Common/GameInstance.h"
typedef std::map<std::string,Res::ResBase*> ResReg;

class ResourceManager
{
public:
    static ResourceManager& Get();
    static void Init();
    Mesh* TransMesh(Res::ResMesh* resMesh);
    StandardMaterial* TransMaterial(Res::ResMaterial* resMaterial);
    Texture* TransTexture(Res::ResTexture* resTexture);
    std::string LoadResource(std::string path);
    void CompileModel(GameInstance* instance,Res::ResModel* model);
public:
    static std::unique_ptr<ResourceManager> Pinstance;
    ResReg resReg;
private:
    void AsynCompile(GameInstance* instance,Res::ResModel* model);
    ResourceManager();

};


#endif //INSTRENDERER_RESOURCEMANAGER_H
