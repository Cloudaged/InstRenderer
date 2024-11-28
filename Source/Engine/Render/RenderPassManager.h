
#ifndef INSTRENDERER_RENDERPASSMANAGER_H
#define INSTRENDERER_RENDERPASSMANAGER_H
#include "Pass/CompositionPass.h"
#include "Pass/GeoPass.h"
#include "../Common/Scene.h"
#include "Pass/PresentPass.h"
#include "Pass/ShadowPass.h"
class RenderSystem;
class RenderPassManager
{
    friend class RenderSystem;
public:
    RenderPassManager();
    void ExecuteAllPass();
    void Setup();

    void RecreateAllPass();
    void ClearAtt();
    entt::view<entt::get_t<Renderable,Transform>> view;
    GlobalDescriptorData globalDescriptorData;
private:
    std::unique_ptr<CompositionPass> compositionPass;
    std::unique_ptr<GeoPass> geoPass;
    std::unique_ptr<PresentPass> presentPass;
    std::unique_ptr<ShadowPass> shadowPass;
};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
