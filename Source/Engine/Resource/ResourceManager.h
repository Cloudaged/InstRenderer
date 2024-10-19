
#ifndef INSTRENDERER_RESOURCEMANAGER_H
#define INSTRENDERER_RESOURCEMANAGER_H
#include "map"
#include "ResBase.h"
#include "filesystem"
#include "ModelLoader.h"
#include "memory"
#include "../Common/Core/PathDefine.h"
typedef std::map<std::string,Res::ResBase*> ResReg;

class ResourceManager
{
public:
    static ResourceManager& Get();
    static void Init();
    ResReg resReg;
    void LoadResource(std::string path);
    static std::unique_ptr<ResourceManager> Pinstance;
private:
    ResourceManager();

};


#endif //INSTRENDERER_RESOURCEMANAGER_H
