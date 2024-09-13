
#ifndef INSTRENDERER_GEOMETRYPASS_H
#define INSTRENDERER_GEOMETRYPASS_H
#include "RenderPass.h"

struct GeoPassPerObjData
{
    glm::vec3 color;
};

class GeometryPass :public RenderPass
{
public:
    GeometryPass(GlobalDescriptorData data);
    void SetupAttachments() override;
    void Execute(entt::view<entt::get_t<Renderable>> view) override;
private:
    void SetupRenderState() override;
    AttachmentData presentData;
    GlobalDescriptorData globalData;
    GeoPassPerObjData perData;
};


#endif //INSTRENDERER_GEOMETRYPASS_H
