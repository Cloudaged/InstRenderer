
#ifndef INSTRENDERER_GEOPASS_H
#define INSTRENDERER_GEOPASS_H
#include "RenderPass.h"
struct GeoPassPerObjData
{
    glm::mat4 modelMatrix;
};

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

    glm::mat4 GetModelMatrixFromTrans(Transform trans);
    GlobalDescriptorData globalData;
    GeoPassPerObjData perData;
};



#endif //INSTRENDERER_GEOPASS_H
