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

public float3 F_Schlick(float3 F0, float VoN)
{
    return F0 + (1 - F0) * pow(1-VoN, 5);
}

public float3 F_SchlickR(float3 F0, float VoN, float roughness)
{
    return F0 + (max(float3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - VoN, 0.0, 1.0), 5.0);
}

public float3 ImportanceSampleGGX(float2 xi, float3 normal,float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * xi.x;
    float cosTheta = sqrt(1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    float3 h;
    h.x = cos(phi) * sinTheta;
    h.y = sin(phi) * sinTheta;
    h.z = cosTheta;

    float3 up = float3(0, 0, 1);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);

    float3 sampleVec = tangent * h.x + bitangent * h.y + normal * h.z;
    return normalize(sampleVec);
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

    float NoL = clamp(dot(N, L), 0.0001, 1);//avoid to divide zero
    float NoV = clamp(dot(N, V), 0.0001, 1);
    float VoH = clamp(dot(V, H), 0.0001, 1);
    float NoH = clamp(dot(N, H), 0.0001, 1);

    // float NoL = dot(N, L);
    // float NoV = dot(N, V);
    // float VoH = dot(V, H);
    // float NoH = dot(N, H);

    float3 F = F_Schlick(F0, NoV);
    float D = D_GGX(roughness, NoH);
    float G = G_SchlickGGX(roughness, NoV)*G_SchlickGGX(roughness,NoL);

    return float3(F * D * G) / 4 * NoL * NoV;
}