import ShaderModule.BasicFunc;
import ShaderModule.PBR;
import ShaderModule.Types;
struct FragInput
{
    float2 inUV : TEXCOORD0;
}


[[vk::binding(0, 0)]] Sampler2D inputPosition;
[[vk::binding(1, 0)]] Sampler2D inputNormal;
[[vk::binding(2, 0)]] Sampler2D inputAlbedo;
[[vk::binding(3, 0)]] Sampler2D inputMR;

[[vk::binding(1, 1)]] ConstantBuffer<LightCameraUniform> lightUniform;

[shader("fragment")]
float4 main(FragInput input) : SV_Target
{
    float4 posWS = inputPosition.Sample(input.inUV);
    float4 normal = normalize(inputNormal.Sample(input.inUV));
    float4 baseColor = inputAlbedo.Sample(input.inUV);
    float4 metallicRoughness = inputMR.Sample(input.inUV);

    float metallic = metallicRoughness.g;
    float roughness = metallicRoughness.r;

    float3 F0 = GetF0(baseColor.xyz, metallicRoughness.x);
    float3 ks = F0;
    float3 kd = float3(1.0) - F0;

    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);

    float3 view = lightUniform.cameraToward.xyz;

    float3 finalColor = float3(0.0f);
    for (int i = 0; i < lightUniform.lightCount; i++)
    {
        Light light = lightUniform.light[i];

        float3 specularBRDF = GetSpecularBRDF(normal.xyz, light.toward.xyz, view, roughness, F0);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
        finalColor += light.intensity * dot(normal.xyz, light.toward.xyz) * finalBRDF;
    }
    return float4(finalColor,1.0);
}