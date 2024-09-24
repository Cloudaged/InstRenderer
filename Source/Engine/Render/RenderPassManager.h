
#ifndef INSTRENDERER_RENDERPASSMANAGER_H
#define INSTRENDERER_RENDERPASSMANAGER_H
#include "Pass/CompositionPass.h"

#include "../Common/Scene.h"

class RenderPassManager
{
public:
    RenderPassManager();
    void ExecuteAllPass();
    void Build();
    void Setup();
    entt::view<entt::get_t<Renderable,Transform>> view;

    GlobalDescriptorData globalDescriptorData;

    CompositionPass* compositionPass;

};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
