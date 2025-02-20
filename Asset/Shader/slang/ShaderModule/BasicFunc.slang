module BasicFunc;


public float4 GetRed(float test)
{
    return float4(1.0, 0.0, 0.0, 0.0);
}

public float3 UnpackNormalMap(float3 normalColor, float3 tangent, float3 bitangent, float3 normal)
{
    float3x3 tbn = float3x3(tangent, bitangent, normal);
    tbn = transpose(tbn);
    normalColor = normalize(mul(tbn, normalColor));
    return normalColor;
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

public float2 Hammersley(uint index, uint samplesCount)
{
    return float2(float(index) / float(samplesCount), RadicalInverse_VdC(index));
}

public float4 SunColor(float3 dir)
{
    float3 floor = float3(1, 0, 0);
    float timeOfDay = dot(floor, dir);


    float4 morningColor = float4(0.2, 0.3, 0.35,0.5); 
    float4 noonColor = float4(1.0, 1.0, 1.0,1.5);    
    float4 eveningColor = float4(1.0, 0.6, 0.3,0.5);

    float4 sunColor = lerp(morningColor, noonColor, smoothstep(-1, 0, timeOfDay));
    sunColor = lerp(sunColor, eveningColor, smoothstep(0, 1.0, timeOfDay));

    return sunColor;
}