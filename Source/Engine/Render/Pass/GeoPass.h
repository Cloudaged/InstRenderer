
#ifndef INSTRENDERER_GEOPASS_H
#define INSTRENDERER_GEOPASS_H
#include "RenderPass.h"
#include "../../Common/Core/PathDefine.h"
#include "../../Common/Core/EngineMath.h"

struct GeoPassPerObjData
{
    glm::mat4 modelMatrix;
};

class GeoPass : public RenderPass
{
public:
    GeoPass(GlobalDescriptorData data);
    void Execute(entt::view<entt::get_t<Renderable,Transform>> view);
    VkDescriptorSetLayout globalLayout;
    VkDescriptorSetLayout materialLayout;
    VkDescriptorSetLayout perObjLayout;
private:
    void SetupRenderState() override;
    void SetupAttachments() override;
    void InputGlobalDesLayout();
    void CreatePerMaterialLayout(std::vector<DescriptorBindingSlot> bindings);
    void CreatePerObjLayout(std::vector<DescriptorBindingSlot> bindings);

    void CreatePerObjDescriptor(size_t uniformSize);

    VkDescriptorSet perObjDes;
    Buffer perObjDesBuffer;


    /*std::shared_ptr<Texture> baseColorAttachment;
    std::shared_ptr<Texture> normalAttachment;
    std::shared_ptr<Texture> positionAttachment;
    std::shared_ptr<Texture> depthAttachment;
    std::shared_ptr<Texture> mrAttachment;*/

    //glm::mat4 GetModelMatrixFromTrans(Transform trans);
    GlobalDescriptorData globalData;
    GeoPassPerObjData perData={};
};



#endif //INSTRENDERER_GEOPASS_H
