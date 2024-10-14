
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
    CompositionPass();
    void SetupAttachments() override;
    void Execute();
private:

    //VkDescriptorSetLayout

    void SetupRenderState() override;
    GlobalDescriptorData globalData;
    CompPassPerObjData perData;

};


#endif //INSTRENDERER_COMPOSITIONPASS_H
