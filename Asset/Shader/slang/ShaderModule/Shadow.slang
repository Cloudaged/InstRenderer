module Shadow;
import Light;
import GraphicSetting;
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_RINGS 10

#define MAX_SMAPLE_COUNT 32
#define BLOCKER_SEARCH_COUNTS 16
#define PCF_SAMPLE_COUNTS 32

// static const int BLOCKER_SEARCH_COUNTS = 32;
// static const int PCF_SAMPLE_COUNTS = 32;
static float2 poissonDisk[MAX_SMAPLE_COUNT];

static float searchRange = 0.1;
float rand_2to1(float2 uv) {
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, float2(a, b));
    float sn = fmod(dt, PI);
    return frac(sin(sn) * c);
}

void poissonDiskSamples(const in float2 randomSeed) {

    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(MAX_SMAPLE_COUNT);
    float INV_NUM_SAMPLES = 1.0 / float(MAX_SMAPLE_COUNT);

    float angle = rand_2to1(randomSeed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < MAX_SMAPLE_COUNT; i++) {
        poissonDisk[i] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

float GetFilterAvgDepth(Sampler2D shadowMap, float2 screenPos, float shadingPointDepth)
{
    poissonDiskSamples(screenPos.xy);
    
    float totalDepth = 0.0;
    float blockerCount = 0.0;
    float filterSize = 2.0;
    float textureSize = 2048.0f;
    float range = filterSize / textureSize;
    for (int i = 0; i < BLOCKER_SEARCH_COUNTS; i++)
    {
        //float range = 0.002;

        float2 offset = poissonDisk[i] * searchRange;
        float lightCamDepth = shadowMap.Sample(screenPos+offset).r;
        if (shadingPointDepth-0.001 > lightCamDepth)
        {
            totalDepth += lightCamDepth;
            blockerCount += 1;
        }
    }
    if(blockerCount == 0)
    {
        return 1.0f;
    }
    return totalDepth / blockerCount;
}

public float GetAvgBlockerDepth(Sampler2D shadowMap, float4x4 lightMat, float4 worldPos)
{
    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 sceenPos = posInLightNDC.xy * 0.5 + 0.5;

    float viewCamDepthLS = posInLightNDC.z;
    return GetFilterAvgDepth(shadowMap, sceenPos, viewCamDepthLS);
}


public float NormalBias(float biasValue,float3 normal,float3 lightDir)
{

    float sinVlaue = length(cross(normal, lightDir));
    float bias = biasValue * max(sinVlaue,0.0f) ;
    return bias;
}

float GetPenmubraSize(float receiverDepth, float avgBlockerDepth,float lightSize)
{
    return lightSize * (receiverDepth - avgBlockerDepth) / avgBlockerDepth;
}

float PCF(Sampler2D shadowMap,float2 screenPos, float camDepth,float trange,float bias)
{
    poissonDiskSamples(screenPos.xy);
    // int sampleCount = max(globShadowSetting.pcfSampleCount, MAX_SMAPLE_COUNT);
    int sampleCount = 32;
    float sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 offset = poissonDisk[i];
        float lightCamDepth = shadowMap.Sample(screenPos + offset*trange).r;
        if (camDepth - 0.001 < lightCamDepth)
        {
            sum += 1;
        }
    }
    return sum / sampleCount;
}

float PCSS(Sampler2D shadowMap, float avgBlockerDepth, float2 screenPos, float camDepth,float lightSize)
{
    float p = GetPenmubraSize(camDepth,avgBlockerDepth,lightSize); 
    float filterSize = 2.0;
    float textureSize = 2048.0f;
    float range = p*filterSize/textureSize ;
    float shadow = PCF(shadowMap, screenPos, camDepth, range, 0.001);
    return shadow;
}

public float CalculateShadow(Sampler2D shadowMap,float avgBlocker,float4x4 lightMat,float4 worldPos,SoftShadowType softSetting,float lightSize)
{
    // PCF_SAMPLE_COUNTS = shadowSetting.pcfSampleCount;
    // BLOCKER_SEARCH_COUNTS = shadowSetting.blockerSearchCount;
    // NUM_SAMPLES = PCF_SAMPLE_COUNTS;
    //globShadowSetting = shadowSetting;
    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 screenPos = posInLightNDC.xy*0.5+0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float shadow = 0.0f;
    if (softSetting == SoftShadowType::PCF)
    {
        shadow = PCF(shadowMap, screenPos, viewCamDepthLS, 0.002, 0.001);
    }
    if (softSetting == SoftShadowType::PCSS)
    {
        shadow = PCSS(shadowMap, avgBlocker, screenPos, viewCamDepthLS,lightSize);
    }
    return shadow;
}

