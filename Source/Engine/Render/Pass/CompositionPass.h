
#ifndef INSTRENDERER_COMPOSITIONPASS_H
#define INSTRENDERER_COMPOSITIONPASS_H
#include "RenderPass.h"

struct CompPassPerObjData
{
    glm::mat4 modelMatrix;
};

class CompositionPass :public RenderPass
{
public:
    CompositionPass(GlobalDescriptorData data);
    void SetupAttachments() override;
    void Execute(entt::view<entt::get_t<Renderable,Transform>> view);
private:
    void SetupRenderState() override;
    glm::mat4 GetModelMatrixFromTrans(Transform trans);
    GlobalDescriptorData globalData;
    CompPassPerObjData perData;
};


#endif //INSTRENDERER_COMPOSITIONPASS_H
