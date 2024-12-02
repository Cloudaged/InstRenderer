module Shadow;

public float CalculateShadow(Sampler2D shadowMap,float4x4 lightMat,float4 worldPos)
{
    float4 posInLightNDC = mul(lightMat, worldPos);
    posInLightNDC = posInLightNDC / posInLightNDC.w;
    float2 sceenPos = posInLightNDC.xy*0.5+0.5;

    float viewCamDepthLS = posInLightNDC.z;
    float lightCamDepthLS = shadowMap.Sample(sceenPos).r;

    if (viewCamDepthLS > lightCamDepthLS)
    {
        return 0.1;
    }
    else
    {
        return 1;
    }
}