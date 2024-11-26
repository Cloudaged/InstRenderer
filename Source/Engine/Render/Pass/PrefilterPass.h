
#ifndef INSTRENDERER_PREFILTERPASS_H
#define INSTRENDERER_PREFILTERPASS_H
#include "RenderPass.h"

class PrefilterPass : public RenderPass
{
public:
    PrefilterPass();
    void SetupAttachments() override;
    void Execute();
private:
    void SetupRenderState() override;
};


#endif //INSTRENDERER_PREFILTERPASS_H
