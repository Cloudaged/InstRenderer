import ShaderModule.BasicFunc;
import ShaderModule.PBR;
import ShaderModule.Types;
import ShaderModule.Shadow;
import ShaderModule.Light;
import ShaderModule.GraphicSetting;
struct FragInput
{
    float2 inUV : TEXCOORD0;
    float4x4 lightViewMat : TEXCOORD1;
    float4x4 lightProjMat : TEXCOORD2;
}


[[vk::binding(0, 0)]] Sampler2D inputPosition;
[[vk::binding(1, 0)]] Sampler2D inputNormal;
[[vk::binding(2, 0)]] Sampler2D inputAlbedo;
[[vk::binding(3, 0)]] Sampler2D inputMR;
[[vk::binding(4, 0)]] Sampler2D shadowMap;

[[vk::binding(1, 1)]] ConstantBuffer<LightCameraUniform> lightUniform;
//[[vk::binding(2, 1)]] ConstantBuffer<GraphicSetting> graphicSetting;

[shader("fragment")]
float4 main(FragInput input) : SV_Target
{
    float4 posWS = inputPosition.Sample(input.inUV);
    float4 normal = normalize(inputNormal.Sample(input.inUV));
    float4 baseColor = inputAlbedo.Sample(input.inUV);
    float4 metallicRoughness = inputMR.Sample(input.inUV);
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    float3 F0 = GetF0(baseColor.xyz, metallic);
    float3 ks = F0;
    float3 kd = float3(1.0) - F0;

    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);

    float3 view = normalize(lightUniform.cameraPos.xyz-posWS.xyz);

    float3 finalColor = float3(0.0f);
    float shading = 0;
    float shadow = 0;
    float blocker = 0;
    for (int i = 0; i < lightUniform.lightCount; i++)
    {
        Light light = lightUniform.light[i];
        LightProperty lp = GetLightProperty(light, posWS.xyz);
        shading = lp.intensity;
        float3 specularBRDF = GetSpecularBRDF(normal.xyz, lp.lightDir, view, roughness, F0);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
        finalColor += lp.intensity * dot(normal.xyz, lp.lightDir.xyz) * finalBRDF;
        
        // finalColor = specularBRDF;
        shadow = CalculateShadow(shadowMap, 
            mul(input.lightProjMat, input.lightViewMat),
             posWS,
             lp.softShadowType,
             light.lightSize);
    }

    return float4(finalColor*shadow,1.0);
    // return testValues;
    //return float4(0.0f);
    //return float4(shadow);
}