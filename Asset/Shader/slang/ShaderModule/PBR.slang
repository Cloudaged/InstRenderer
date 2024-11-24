module PBR;
import BasicFunc;
import Types;
public float3 GetF0(float3 albedo,float metallic)
{
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    return F0;

}

float3 GetDiffuseBRDF(float3 albedo)
{
    return albedo * (1 / PI);
}

float3 GetSpecularBRDF(float3 N, float3 L, float3 V, float roughness, float3 F0)
{
    
}