
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
    const int cascadedCount = 4;
    void InitCSM();
    std::vector<glm::vec3> GetFrustumCornersWS(const std::vector<glm::vec3>& ndcCorners,const glm::mat4& vMat,const glm::mat4& pMat);
    glm::mat4 modelMatrix;
    uint32_t shadowMapWidth,shadowMapHeight;
    GlobalDescriptorData globalDescriptorData;
};


#endif //INSTRENDERER_SHADOWPASS_H
