
#ifndef INSTRENDERER_RENDERPASSMANAGER_H
#define INSTRENDERER_RENDERPASSMANAGER_H
#include "Pass/CompositionPass.h"
#include "Pass/GeoPass.h"
#include "../Common/Scene.h"

class RenderPassManager
{
public:
    RenderPassManager();
    void ExecuteAllPass();
    void Build();
    void Setup();

    void RecreateAllPass();
    void ClearAtt();
    entt::view<entt::get_t<Renderable,Transform>> view;

    GlobalDescriptorData globalDescriptorData;

    CompositionPass* compositionPass;
    GeoPass* geoPass;
};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
