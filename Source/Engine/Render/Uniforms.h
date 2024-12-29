
#ifndef INSTRENDERER_UNIFORMS_H
#define INSTRENDERER_UNIFORMS_H
#include "glm/glm.hpp"
#include "Buffer/Skybox.h"

struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 skyboxProj;
    glm::mat4 lightViewMat;
    glm::mat4 lightProjMat;
};



struct LightUnitsInShader
{
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec4 toward;
    alignas(16) glm::vec4 color;
    alignas(4) int type;
    alignas(4) float intensity;
    alignas(4) float range;
    alignas(4) float outerCutoff;
    alignas(4) float attenuation;
};

struct LightUniform
{
    alignas(16) glm::vec4 cameraPos;
    alignas(16) glm::vec4 cameraDir;
    alignas(4) int count;
    LightUnitsInShader lights[16];
};


struct GlobalDescriptorData
{
    Buffer globBuffer;
    Buffer lightBuffer;
    Buffer graphicBuffer;
    VkDescriptorSetLayout globalDesLayout;
    VkDescriptorSet globalDes;
    Skybox* skyboxData;
};

struct ShadowSetting
{
    int pcfSampleCount = 32;
    int blockerSearchCount = 32;
};

struct RenderSettingUniform
{
    ShadowSetting shadowDebug{};
};


#endif //INSTRENDERER_UNIFORMS_H
