
#ifndef INSTRENDERER_PRESENTPASS_H
#define INSTRENDERER_PRESENTPASS_H
#include "RenderPass.h"

class PresentPass : public RenderPass
{
public:
    PresentPass();
    void SetupAttachments() override;
    void Execute();
private:
    void SetupRenderState() override;
};


#endif //INSTRENDERER_PRESENTPASS_H
