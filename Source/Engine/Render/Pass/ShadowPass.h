
#ifndef INSTRENDERER_SHADOWPASS_H
#define INSTRENDERER_SHADOWPASS_H
#include "RenderPass.h"
class ShadowPass : public RenderPass
{
public:
    ShadowPass(GlobalDescriptorData globalDescriptorData);
    void SetupRenderState() override;
    void SetupAttachments() override;
    void Execute(entt::view<entt::get_t<Renderable,Transform>> compView);
private:
    uint32_t shadowMapWidth,shadowMapHeight;
    GlobalDescriptorData globalDescriptorData;
    Texture* shadowMapAttachment;
};


#endif //INSTRENDERER_SHADOWPASS_H
