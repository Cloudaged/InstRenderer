
#ifndef INSTRENDERER_COMPOSITIONPASS_H
#define INSTRENDERER_COMPOSITIONPASS_H
#include "RenderPass.h"
class CompositionPass :public RenderPass
{
public:
    CompositionPass(GlobalDescriptorData globData);
    void SetupAttachments() override;
    void Execute();
private:
    void CreateSkyboxDes();
    GlobalDescriptorData globData;
    RenderState skyboxRenderState;
    VkDescriptorSetLayout skyboxLayout;
    VkDescriptorSet skyboxDescriptor;
    void SetupRenderState() override;

};


#endif //INSTRENDERER_COMPOSITIONPASS_H
