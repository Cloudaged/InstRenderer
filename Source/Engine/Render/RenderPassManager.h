
#ifndef INSTRENDERER_RENDERPASSMANAGER_H
#define INSTRENDERER_RENDERPASSMANAGER_H
#include "Pass/CompositionPass.h"
#include "Pass/GeoPass.h"
#include "../Common/Scene.h"
#include "Pass/PresentPass.h"
#include "Pass/SkyboxPass.h"
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
    PresentPass* presentPass;
    SkyboxPass* skyboxPass;
};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
