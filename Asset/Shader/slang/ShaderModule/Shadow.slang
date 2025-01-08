module Shadow;
import Light;
import Types;
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_RINGS 10

#define MAX_SMAPLE_COUNT 64
// #define BLOCKER_SEARCH_COUNTS 16
// #define PCF_SAMPLE_COUNTS 32

 static int BLOCKER_SEARCH_COUNTS = 32;
 static int PCF_SAMPLE_COUNTS = 32;
static float2 poissonDisk[MAX_SMAPLE_COUNT];

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

public float GetAvgBlockerDepth(Sampler2D shadowMap,float2 screenPos,float camDepthLS)
{
    poissonDiskSamples(screenPos.xy);
    float searchRange = 0.1;

    float totalDepth = 0.0;
    float blockerCount = 0.0;
    for (int i = 0; i < BLOCKER_SEARCH_COUNTS; i++)
    {
        // float range = 0.002;
        float2 samplePos = screenPos + poissonDisk[i] * searchRange;
        if (samplePos.x < 0.0 || samplePos.x > 1.0 || samplePos.y < 0.0 || samplePos.y > 1.0) {
            continue;
        }
        float lightCamDepth = shadowMap.Sample(samplePos).r;
        if (camDepthLS > lightCamDepth)
        {
            totalDepth += lightCamDepth;
            blockerCount += 1;
        }
    }
    if (blockerCount == 0)
    {
        return 1.0f;
    }
    return totalDepth / blockerCount;

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

float PCF(Sampler2D shadowMap,float2 screenPos, float camDepth,float range,float bias)
{
    poissonDiskSamples(screenPos.xy);
    // int sampleCount = max(globShadowSetting.pcfSampleCount, MAX_SMAPLE_COUNT);
    int sampleCount = PCF_SAMPLE_COUNTS;
    float sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 samplePos = screenPos + poissonDisk[i] * range;
        if (samplePos.x < 0.0 || samplePos.x > 1.0 || samplePos.y < 0.0 || samplePos.y > 1.0) {
            sum += 1;
            continue;
        }
        float lightCamDepth = shadowMap.Sample(samplePos).r;
        if (camDepth - 0.001 < lightCamDepth)
        {
            sum += 1;
        }
    }
    return sum / sampleCount;
}

float PCF_CSM(Sampler2DArray shadowMapArr,int index, float2 screenPos, float camDepth, float range, float bias)
{
    poissonDiskSamples(screenPos.xy);
    // int sampleCount = max(globShadowSetting.pcfSampleCount, MAX_SMAPLE_COUNT);
    int sampleCount = 32;
    float sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 samplePos = screenPos + poissonDisk[i] * range;
        if (samplePos.x < 0.0 || samplePos.x > 1.0 || samplePos.y < 0.0 || samplePos.y > 1.0) {
            sum += 1;
            continue;
        }
        float lightCamDepth = shadowMapArr.Sample((samplePos,index)).r;
        if (camDepth - 0.001 < lightCamDepth)
        {
            sum += 1;
        }
    }        

    return sum / sampleCount;
}


float PCSS(Sampler2D shadowMap, float2 screenPos, float camDepth,float lightSize)
{
    float avgBlockerDepth = GetAvgBlockerDepth(shadowMap,screenPos, camDepth);

    float p = GetPenmubraSize(camDepth,avgBlockerDepth,lightSize); 
    float filterSize = 2.0;
    float textureSize = 2048.0f;
    float range = p*filterSize/textureSize ;
    float shadow = PCF(shadowMap, screenPos, camDepth, range, 0.001);
    return shadow;
}

public float CalculateShadow(Sampler2D shadowMap,float4x4 lightMat,float4 worldPos,SoftShadowType softSetting,float lightSize,ShadowSetting shadowSetting)
{
     PCF_SAMPLE_COUNTS = shadowSetting.pcfSampleCount;
     BLOCKER_SEARCH_COUNTS = shadowSetting.blockerSearchCount;
    // NUM_SAMPLES = PCF_SAMPLE_COUNTS;
    // globShadowSetting = shadowSetting;

    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 screenPos = posInLightNDC.xy*0.5+0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float shadow = 0.0f;

    if (screenPos.x < 0.0 || screenPos.x > 1.0 || screenPos.y < 0.0 || screenPos.y > 1.0) {
        return 0.0;
    }

    if (softSetting == SoftShadowType::PCF)
    {
        shadow = PCF(shadowMap, screenPos, viewCamDepthLS, 0.002, 0.001);
    }
    if (softSetting == SoftShadowType::PCSS)
    {
        shadow = PCSS(shadowMap, screenPos, viewCamDepthLS,lightSize);
    }
    return shadow;
}

public float CSM(Sampler2DArray shadowMapArr,int index,float4x4 lightMat, float4 worldPos, SoftShadowType softSetting, float lightSize, ShadowSetting shadowSetting)
{
    PCF_SAMPLE_COUNTS = shadowSetting.pcfSampleCount;
    BLOCKER_SEARCH_COUNTS = shadowSetting.blockerSearchCount;
    // NUM_SAMPLES = PCF_SAMPLE_COUNTS;
    // globShadowSetting = shadowSetting;

    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 screenPos = posInLightNDC.xy * 0.5 + 0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float shadow = 0.0f;

    if (screenPos.x < 0.0 || screenPos.x > 1.0 || screenPos.y < 0.0 || screenPos.y > 1.0) {
        return 0.0;
    }

    float lightCamDepth = shadowMapArr.Sample(float3(screenPos.xy, index)).r;
    if (viewCamDepthLS - 0.001 <= lightCamDepth)
    {
        shadow = 1;
    }

    //shadow = PCF_CSM(shadowMapArr, index,screenPos, viewCamDepthLS, 0.002, 0.001);

    return shadow;
}

public float4 ShowCascade(int index)
{
    switch (index)
    {
    case 0: return float4(1, 0, 0, 1);
    case 1: return float4(0, 1, 0, 1);
    case 2: return float4(0, 0, 1, 1);
    case 3: return float4(1, 1, 0, 1);
    default: return float4(0, 0, 0, 0);
    }
}