
#ifndef INSTRENDERER_RENDERPASSMANAGER_H
#define INSTRENDERER_RENDERPASSMANAGER_H
#include "Pass/GeometryPass.h"

#include "../Common/Scene.h"

class RenderPassManager
{
public:
    RenderPassManager();
    void ExecuteAllPass();
    void Build();
    void Setup();
    entt::view<entt::get_t<Renderable>> view;

    GlobalDescriptorData globalDescriptorData;

    GeometryPass* geometryPass;

};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
