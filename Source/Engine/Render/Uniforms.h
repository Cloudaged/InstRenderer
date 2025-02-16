
#ifndef INSTRENDERER_UNIFORMS_H
#define INSTRENDERER_UNIFORMS_H
#include "../Common/Core/glmConfig.h"
#include "../Render/Material/Material.h"

#define CASCADED_COUNT  4
#define CASCADED_WIDTH 1024
#define CASCADED_HEIGHT 1024
#define SSAO_ROTATION_SIZE 4
#define PROBE_AREA_SIZE 16
#define RAYS_PER_PROBE 256
#define IRRADIANCE_VOLUME_SIZE 8

struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 skyboxProj;
    glm::mat4 lightViewMat;
    glm::mat4 lightProjMat;
    float nearPlane;
    float farPlane;
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
    int showCascade = -2;
    int antiShimmering = 1;
    int enablePCF = 1;
};

struct DDGISetting
{
    int probeVisualized;
};

struct RenderSettingUniform
{
    ShadowSetting shadowDebug{};
    DDGISetting ddgiSetting{};
};

struct CSMUniform
{
     glm::mat4 viewProjMat[CASCADED_COUNT];
     glm::vec4 cascadeSplits[CASCADED_COUNT];
     glm::vec4 unitPerPix[CASCADED_COUNT];
     int width;
     int height;
     float radiusBias;
};

struct GeometryNode
{
    VkDeviceAddress vertBufferAddress;//8
    VkDeviceAddress indexBufferAddress;//8
    VkDeviceAddress transformAddress;//8
    VkDeviceAddress pad0=0;//8
    Material material;//16
};

struct GeometryNodeArr
{
    alignas(16) GeometryNode nodes[300];
};


struct SSAOKernels
{
    glm::vec4 kernels[32];
};

struct RTUniform
{
    glm::mat4 invView;
    glm::mat4 invProj;
    glm::vec4 camRight;
    glm::vec4 camUp;
};
struct Probe
{
    glm::vec4 position;
};


struct ProbeArea
{
    Probe probes[PROBE_AREA_SIZE*PROBE_AREA_SIZE*PROBE_AREA_SIZE];
};
struct ProbeVisualSphere
{
    VkDeviceAddress vertAddress;
    VkDeviceAddress indicesAddress;
    int vertCount;
    int indicesCount;
};
#endif //INSTRENDERER_UNIFORMS_H
