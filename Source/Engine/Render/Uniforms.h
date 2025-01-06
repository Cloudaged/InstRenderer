
#ifndef INSTRENDERER_UNIFORMS_H
#define INSTRENDERER_UNIFORMS_H
#include "../Common/Core/glmConfig.h"
#define CASCADED_COUNT  4
#define CASCADED_WIDTH 512
#define CASCADED_HEIGHT 512
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


struct ShadowSetting
{
    int pcfSampleCount = 32;
    int blockerSearchCount = 32;
};

struct RenderSettingUniform
{
    ShadowSetting shadowDebug{};
};

struct CSMUniform
{
     glm::mat4 viewProjMat[CASCADED_COUNT];
     glm::vec4 cascadeSplits[CASCADED_COUNT];
     int width;
     int height;
};


#endif //INSTRENDERER_UNIFORMS_H
