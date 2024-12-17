
#include "RenderSystem.h"
#include "../../Render/VulkanContext.h"
RenderSettingData globalRenderSettingData{};

void RenderSystem::BeginSystem(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    UpdateLightArray();
    InitGlobalDescriptorSet();
    passManager.Setup(scene->reg.view<Renderable,Transform>(),globalData);
    materialManager.Setup(passManager.geoPass->materialLayout,scene->reg.view<Renderable>());
    VulkanContext::GetContext().passManager = &this->passManager;
}

void RenderSystem::Execute()
{
    PrepareData();
    VulkanContext::GetContext().DrawPrepare();
    passManager.ExecuteAllPass();
    VulkanContext::GetContext().Submit();
}

RenderSystem::RenderSystem()
{
    passManager = RenderPassManager();
}

void RenderSystem::PrepareData()
{
    PrepareGlobal();
    PrepareLight();
    MemoryCopy();
}

void RenderSystem::UpdateLightArray()
{
    int num=0;
    auto view= scene->reg.view<LightComponent,Transform>();
    for (auto& entityID:view)
    {
        auto& lightComp= view.get<LightComponent>(entityID);
        auto& trans = view.get<Transform>(entityID);
        glm::mat4 mat = EngineMath::GetRotateMatrix(trans.rotation);
        glm::vec4 rotatedDir = mat*glm::vec4(0.0,0.0,1.0,0.0);
        lightUniform.lights[num] = std::move(LightUnitsInShader{glm::vec4(trans.pos,1.0),rotatedDir,
                                                                glm::vec4(lightComp.color,1.0),
                                                                (int)lightComp.type,lightComp.Intensity,
                                                                glm::radians(lightComp.range),lightComp.lightSize,lightComp.attenuation});
        num++;
    }
    lightUniform.count=num;
}

void RenderSystem::PrepareLight()
{

    lightUniform.cameraPos= glm::vec4(scene->mainCamera.position,1.0);
    lightUniform.cameraDir = glm::vec4(scene->mainCamera.viewPoint-scene->mainCamera.position,1.0);
}

void RenderSystem::PrepareGlobal()
{
    globalUniform.skyboxProj = glm::perspective(glm::radians(80.0f),
                                                scene->mainCamera.cameraData.aspect,
                                                0.001f, 256.0f);
    globalUniform.skyboxProj[1][1] *=-1;
    globalUniform.view = scene->mainCamera.vpMat.view;
    globalUniform.proj = scene->mainCamera.vpMat.proj;

    auto [lightV,lightP] = scene->mainLight->GetLightMatrix(&scene->reg,scene->minPoint,scene->maxPoint);
    globalUniform.lightViewMat = lightV;
    globalUniform.lightProjMat = lightP;
}

void RenderSystem::MemoryCopy()
{
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.globBuffer),&globalUniform,sizeof(GlobalUniform));
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.lightBuffer),&lightUniform,sizeof(LightUniform));
    memcpy(VulkanContext::GetContext().bufferAllocator.GetMappedMemory(globalData.graphicBuffer),&globalRenderSettingData,sizeof(globalRenderSettingData));

}

void RenderSystem::SetupRenderGraph()
{
}

void RenderSystem::InitGlobalDescriptorSet()
{
    globalData.skyboxData = scene->skybox.get();

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding vp;
    vp.binding=0;
    vp.descriptorCount=1;
    vp.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorSetLayoutBinding light;
    light.binding=1;
    light.descriptorCount=1;
    light.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    light.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorSetLayoutBinding graphicSettingBinding;
    graphicSettingBinding.binding=2;
    graphicSettingBinding.descriptorCount=1;
    graphicSettingBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    graphicSettingBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    bindings.push_back(vp);
    bindings.push_back(light);
    bindings.push_back(graphicSettingBinding);
    //Layout

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(VulkanContext::GetContext().device, &layoutInfo, nullptr,
                                    &globalData.globalDesLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ds layout!");
    }

    //Allocate Descriptor
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = VulkanContext::GetContext().pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &globalData.globalDesLayout;

    if (vkAllocateDescriptorSets(VulkanContext::GetContext().device, &allocateInfo,&globalData.globalDes) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate ds");
    }
    //Allocate
    int a = sizeof(lightUniform);
    globalData.globBuffer= *VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(globalUniform),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    globalData.lightBuffer =*VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(LightUniform),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    globalData.graphicBuffer = *VulkanContext::GetContext().bufferAllocator.CreateBuffer(sizeof(RenderSettingData),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

    //std::vector<VkDescriptorBufferInfo> bufferInfos;
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer =globalData.globBuffer.vk_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(globalUniform);

    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer =globalData.lightBuffer.vk_buffer;
    lightBufferInfo.offset = 0;
    lightBufferInfo.range = sizeof(LightUniform);

    VkDescriptorBufferInfo graphicSettingsInfo{};
    graphicSettingsInfo.buffer =globalData.graphicBuffer.vk_buffer;
    graphicSettingsInfo.offset = 0;
    graphicSettingsInfo.range = sizeof(RenderSettingData);

    std::vector<VkWriteDescriptorSet> writes;
    VkWriteDescriptorSet descriptorWrites1{};
    descriptorWrites1.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites1.dstSet =globalData.globalDes;
    descriptorWrites1.dstBinding = 0;
    descriptorWrites1.dstArrayElement = 0;
    descriptorWrites1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites1.descriptorCount = 1;
    descriptorWrites1.pBufferInfo = &bufferInfo;
    writes.push_back(descriptorWrites1);


    VkWriteDescriptorSet descriptorWrites2{};
    descriptorWrites2.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites2.dstSet =globalData.globalDes;
    descriptorWrites2.dstBinding = 1;
    descriptorWrites2.dstArrayElement = 0;
    descriptorWrites2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites2.descriptorCount = 1;
    descriptorWrites2.pBufferInfo = &lightBufferInfo;
    writes.push_back(descriptorWrites2);

    VkWriteDescriptorSet descriptorWrites3{};
    descriptorWrites3.sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3.dstSet =globalData.globalDes;
    descriptorWrites3.dstBinding = 2;
    descriptorWrites3.dstArrayElement = 0;
    descriptorWrites3.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites3.descriptorCount = 1;
    descriptorWrites3.pBufferInfo = &graphicSettingsInfo;
    writes.push_back(descriptorWrites3);

    vkUpdateDescriptorSets(VulkanContext::GetContext().device,writes.size(),writes.data(),0, nullptr);
}








