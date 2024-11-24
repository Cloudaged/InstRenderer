module PBR;
import BasicFunc;
import Types;
public float3 GetF0(float3 albedo,float metallic)
{
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    return F0;

}

public float3 GetDiffuseBRDF(float3 albedo)
{
    return albedo * (1 / PI);
}

float3 F_Schlick(float3 F0, float VoH)
{
    return F0 + (1 - F0) * pow(VoH, 5);
}

float D_GGX(float roughness, float NoH)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NoH * NoH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}

float G_SchlickGGX(float roughness, float NoV)
{
    float k = pow(roughness + 1, 2) / 8;
    float deno = NoV * (1 - k) + k;
    return NoV / deno;
}

public float3 GetSpecularBRDF(float3 N, float3 L, float3 V, float roughness, float3 F0)
{
    float3 H = normalize(V + L);

    // float NoL = clamp(dot(N, L), 0, 1);
    // float NoV = clamp(dot(N, V), 0, 1);
    // float VoH = clamp(dot(V, H), 0, 1);
    // float NoH = clamp(dot(N, H), 0, 1);

    float NoL = dot(N, L);
    float NoV = dot(N, V);
    float VoH = dot(V, H);
    float NoH = dot(N, H);

    float3 F = F_Schlick(F0, VoH);
    float D = D_GGX(roughness, NoH);
    float G = G_SchlickGGX(roughness, NoV)*G_SchlickGGX(roughness,NoL);

    return float3(F * D * G) / (4 * NoL * NoV);
}