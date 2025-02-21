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
 static float2 poissonDisk[MAX_SMAPLE_COUNT] =
      {
         float2(0.6980989, 0.5259584),
         float2(0.43980473, 0.10610481),
         float2(0.57310927, 0.9893704),
         float2(0.24529353, 0.3447856),
         float2(0.03719565, 0.8421317),
         float2(0.9691836, 0.28905225),
         float2(0.32002336, 0.87438554),
         float2(0.79917806, 0.48806584),
         float2(0.8525596, 0.54047424),
         float2(0.40748486, 0.6955889),
         float2(0.6947326, 0.99577194),
         float2(0.3402657, 0.9757582),
         float2(0.09728526, 0.47314274),
         float2(0.8101438, 0.9253465),
         float2(0.6623809, 0.87968296),
         float2(0.87790006, 0.06210297),
         float2(0.7562477, 0.12731777),
         float2(0.9719074, 0.30123645),
         float2(0.6876344, 0.33697855),
         float2(0.18553196, 0.77107185),
         float2(0.45789656, 0.8520921),
         float2(0.47452676, 0.9280548),
         float2(0.12627, 0.3129187),
         float2(0.33199888, 0.62038684),
         float2(0.05571044, 0.52381057),
         float2(0.9481783, 0.22935988),
         float2(0.7040952, 0.82030535),
         float2(0.07485578, 0.31013197),
         float2(0.2044181, 0.6017704),
         float2(0.17348765, 0.02603168),
         float2(0.78817815, 0.8589141),
         float2(0.7107235, 0.47501194),
         float2(0.5955747, 0.73461306),
         float2(0.07158836, 0.107009),
         float2(0.7783702, 0.26512975),
         float2(0.10766187, 0.01240772),
         float2(0.07778926, 0.1744973),
         float2(0.612285, 0.800719),
         float2(0.41252193, 0.54059166),
         float2(0.32328537, 0.11903202),
         float2(0.69910073, 0.8041949),
         float2(0.4592937, 0.212285),
         float2(0.8061171, 0.5050117),
         float2(0.79910994, 0.15456377),
         float2(0.8259469, 0.29158393),
         float2(0.78877807, 0.7652055),
         float2(0.46713823, 0.68777263),
         float2(0.47074047, 0.82612336),
         float2(0.15579747, 0.85885537),
         float2(0.07905079, 0.22726096),
         float2(0.8057656, 0.04124214),
         float2(0.84564483, 0.7511618),
         float2(0.37201563, 0.2265257),
         float2(0.8625208, 0.4302818),
         float2(0.00530847, 0.7861793),
         float2(0.52683383, 0.5268234),
         float2(0.73706037, 0.01161546),
         float2(0.06791341, 0.7281362),
         float2(0.20959325, 0.37239334),
         float2(0.04213179, 0.29816273),
         float2(0.0183489, 0.17338806),
         float2(0.83031744, 0.41574556),
         float2(0.99181575, 0.9364823),
         float2(0.6493541, 0.8105762)
     };

 float rand_2to1(float2 uv) {
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, float2(a, b));
    float sn = fmod(dt, PI);
    return frac(sin(sn) * c);
}

// void poissonDiskSamples(out float2 poissonDisk[MAX_SMAPLE_COUNT],const in float2 randomSeed) {

//     float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(MAX_SMAPLE_COUNT);
//     float INV_NUM_SAMPLES = 1.0 / float(MAX_SMAPLE_COUNT);

//     float angle = rand_2to1(randomSeed) * PI2;
//     float radius = INV_NUM_SAMPLES;
//     float radiusStep = radius;

//     for (int i = 0; i < MAX_SMAPLE_COUNT; i++) {
//         poissonDisk[i] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
//         radius += radiusStep;
//         angle += ANGLE_STEP;
//     }
// }

public float GetAvgBlockerDepth(Sampler2D shadowMap,float2 screenPos,float camDepthLS)
{
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
    int sampleCount = PCF_SAMPLE_COUNTS;
    float sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 samplePos = screenPos + poissonDisk[i] * range;//Line 109

        float lightCamDepth = shadowMapArr.Sample(float3(samplePos, index)).r;
    
        sum += float(camDepth - bias < lightCamDepth);
    }        

    return sum / sampleCount;
}

float PCSS(Sampler2D shadowMap, float2 screenPos, float camDepth,float lightSize)
{
    float avgBlockerDepth = GetAvgBlockerDepth(shadowMap,screenPos, camDepth);

    float p = GetPenmubraSize(camDepth,avgBlockerDepth,lightSize); 
    float filterSize = 2.0;
    float textureSize = 2048.0f;
    float range = p * filterSize / textureSize;
    float shadow = PCF(shadowMap, screenPos, camDepth, range, 0.001);
    return shadow;
}

public float CSM(Sampler2DArray shadowMapArr,int index,int blendIndex,float blendValue,CSMUniform csmU, float4 worldPos, SoftShadowType softSetting, float lightSize, ShadowSetting shadowSetting)
{
    PCF_SAMPLE_COUNTS = shadowSetting.pcfSampleCount;
    BLOCKER_SEARCH_COUNTS = shadowSetting.blockerSearchCount;
    // NUM_SAMPLES = PCF_SAMPLE_COUNTS;
    // globShadowSetting = shadowSetting;
    float rangeFactor = 0.005;

    float4 posInLightNDC = mul(csmU.vpMat[index], worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 screenPos = posInLightNDC.xy * 0.5 + 0.5;
    float viewCamDepthLS = posInLightNDC.z;
    float range = (1.0f / csmU.unitPerPix[index]) * rangeFactor;
    float2 poissonDisk[MAX_SMAPLE_COUNT];

    float4 posInLightNDC1 = mul(csmU.vpMat[blendIndex], worldPos);
    posInLightNDC1 = posInLightNDC1 / posInLightNDC1.w;
    float2 screenPos1 = posInLightNDC1.xy * 0.5 + 0.5;
    float viewCamDepthLS1 = posInLightNDC1.z;
    float range1 = (1.0f / csmU.unitPerPix[blendIndex]) * rangeFactor;

    float shadow = 0.0f;
    if (shadowSetting.enablePCF == 1)
    {
        float result1 = PCF_CSM( shadowMapArr, index, screenPos, viewCamDepthLS, range, 0.001);
        float result2 = PCF_CSM(shadowMapArr, blendIndex, screenPos1, viewCamDepthLS1, range1, 0.001);
        shadow = lerp(result1, result2, blendValue);
    } else
    {
        float result1 = PCF_CSM(shadowMapArr, index, screenPos, viewCamDepthLS, 0, 0.001);
        float result2 = PCF_CSM(shadowMapArr, blendIndex, screenPos1, viewCamDepthLS1, 0, 0.001);
        shadow = lerp(result1, result2, blendValue);
    }
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
