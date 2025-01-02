
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
#include "thread"
#include "../Common/GameInstance.h"
#include "memory"
#include "../Render/Material/Material.h"

typedef std::map<std::string,std::shared_ptr<Res::ResBase>> ResReg;

class ResourceManager
{
public:
    static ResourceManager& Get();
    static void Init();
    Mesh TransMesh(std::shared_ptr<Res::ResMesh> resMesh);
    Material TransMaterial(RDG::RenderGraph& renderGraph,std::shared_ptr<Res::ResMaterial> resMaterial);
    std::shared_ptr<Texture> AllocTexture(std::shared_ptr<Res::ResTexture> resTexture);
    std::string LoadResource(std::string path);
    void CompileModel(GameInstance* instance,std::shared_ptr<Res::ResModel> model);
    std::shared_ptr<GameObject> AddSceneNode(GameInstance* instance,std::shared_ptr<Res::ResNode> node,std::shared_ptr<GameObject> parentGO);
public:
    static std::unique_ptr<ResourceManager> Pinstance;
    ResReg resReg;
private:
    void AsynCompile(GameInstance* instance,std::shared_ptr<Res::ResModel> model);
    ResourceManager();

};


#endif //INSTRENDERER_RESOURCEMANAGER_H
