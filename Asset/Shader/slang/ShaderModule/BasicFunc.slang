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