module Shadow;

public float NormalBias(float biasValue,float3 normal,float3 lightDir)
{
    float bias = biasValue * max(dot(normalize(normal), normalize(lightDir)), 0.0);
    return bias;
}

public float CalculateShadow(Sampler2D shadowMap,float4x4 lightMat,float4 worldPos,float bias)
{
    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 sceenPos = posInLightNDC.xy*0.5+0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float lightCamDepthLS = shadowMap.Sample(sceenPos).r;

    if (viewCamDepthLS+bias > lightCamDepthLS)
    {
        return 0.1;
    }
    else
    {
        return 1;
    }
}