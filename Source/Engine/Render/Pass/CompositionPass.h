
#ifndef INSTRENDERER_COMPOSITIONPASS_H
#define INSTRENDERER_COMPOSITIONPASS_H
#include "RenderPass.h"

class CompositionPass :public RenderPass
{
public:
    CompositionPass();
    void SetupAttachments() override;
    void Execute();
private:

    void SetupRenderState() override;

};


#endif //INSTRENDERER_COMPOSITIONPASS_H
