
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
    void Setup(entt::view<entt::get_t<Renderable,Transform>> view,GlobalDescriptorData globalData);
    void RecreateAllPass();
    void ClearAtt();
private:
    entt::view<entt::get_t<Renderable,Transform>> view;
    GlobalDescriptorData globalDescriptorData;
    std::unique_ptr<CompositionPass> compositionPass;
    std::unique_ptr<GeoPass> geoPass;
    std::unique_ptr<PresentPass> presentPass;
    std::unique_ptr<ShadowPass> shadowPass;
};


#endif //INSTRENDERER_RENDERPASSMANAGER_H
