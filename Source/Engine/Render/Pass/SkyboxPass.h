
#ifndef INSTRENDERER_SKYBOXPASS_H
#define INSTRENDERER_SKYBOXPASS_H
#include "RenderPass.h"

class SkyboxPass : public RenderPass
{
public:
    SkyboxPass(GlobalDescriptorData data);
    void SetupAttachments() override;
    void Execute();
private:
    void CreateDescriptorAndLayout(std::vector<DescriptorBindingSlot> bindings);
    void SetupRenderState() override;
    Texture* skyboxAttachment;
    GlobalDescriptorData globalData;
    VkDescriptorSetLayout imageLayout;
    VkDescriptorSet imageSet;
    Skybox* skybox;
};


#endif //INSTRENDERER_SKYBOXPASS_H
