module Shadow;

static const int PCF_SAMPLE_COUNTS = 16;

static float2 poissonDisk[16] = {
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),
    float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),
    float2(0.14383161, -0.14100790)
};


public float NormalBias(float biasValue,float3 normal,float3 lightDir)
{

    float sinVlaue = length(cross(normal, lightDir));
    float bias = biasValue * max(sinVlaue,0.0f) ;
    return bias;
}

public float PCF(Sampler2D shadowMap, float2 screenPos, float camDepth, float bias)
{
    float sum = 0;
    for (int i = 0; i < PCF_SAMPLE_COUNTS; ++i)
    {
        float2 offset = poissonDisk[i];
        float lightCamDepth = shadowMap.Sample(screenPos + offset*0.001).r;
        if (camDepth - 0.001 < lightCamDepth)
        {
            sum += 1;
        }
    }
    return sum / PCF_SAMPLE_COUNTS;
}

public float CalculateShadow(Sampler2D shadowMap,float4x4 lightMat,float4 worldPos,float bias)
{
    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 sceenPos = posInLightNDC.xy*0.5+0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float lightCamDepthLS = shadowMap.Sample(sceenPos).r;

    float shadow = PCF(shadowMap, sceenPos, viewCamDepthLS, 0.001);
    return shadow;
}

