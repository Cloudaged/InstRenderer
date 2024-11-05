
#ifndef INSTRENDERER_SKYBOXPASS_H
#define INSTRENDERER_SKYBOXPASS_H
#include "RenderPass.h"

class SkyboxPass : public RenderPass
{
public:
    SkyboxPass();
    void SetupAttachments() override;
    void Execute();
private:
    void SetupRenderState() override;


};


#endif //INSTRENDERER_SKYBOXPASS_H
