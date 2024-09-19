
#ifndef INSTRENDERER_GEOMETRYPASS_H
#define INSTRENDERER_GEOMETRYPASS_H
#include "RenderPass.h"

struct GeoPassPerObjData
{
    glm::mat4 modelMatrix;
};

class GeometryPass :public RenderPass
{
public:
    GeometryPass(GlobalDescriptorData data);
    void SetupAttachments() override;
    void Execute(entt::view<entt::get_t<Renderable,Transform>> view);
private:
    void SetupRenderState() override;
    glm::mat4 GetModelMatrixFromTrans(Transform trans);
    AttachmentData presentData;
    GlobalDescriptorData globalData;
    GeoPassPerObjData perData;
};


#endif //INSTRENDERER_GEOMETRYPASS_H
