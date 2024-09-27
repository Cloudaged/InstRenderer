
#ifndef INSTRENDERER_GEOPASS_H
#define INSTRENDERER_GEOPASS_H
#include "RenderPass.h"

class GeoPass : public RenderPass
{
public:
    GeoPass(GlobalDescriptorData data);
    void SetupAttachments() override;
    void Execute(entt::view<entt::get_t<Renderable,Transform>> view);
private:
    void SetupRenderState() override;

    AllocatedImage* baseColorAttachment;
    AllocatedImage* normalAttachment;
    AllocatedImage* positionAttachment;

};


#endif //INSTRENDERER_GEOPASS_H
