
#ifndef INSTRENDERER_BLOCKERSEARCHPASS_H
#define INSTRENDERER_BLOCKERSEARCHPASS_H
#include "RenderPass.h"

class BlockerSearchPass : public RenderPass
{
public:
    BlockerSearchPass(GlobalDescriptorData globalData);
    void SetupAttachments() override;
    void Execute();
private:
    GlobalDescriptorData globalData;
    void SetupRenderState() override;
    Texture* blockerSearchAtt;
};


#endif //INSTRENDERER_BLOCKERSEARCHPASS_H
