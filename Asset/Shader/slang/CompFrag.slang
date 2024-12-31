import ShaderModule.BasicFunc;
import ShaderModule.PBR;
import ShaderModule.Types;
import ShaderModule.Shadow;
import ShaderModule.Light;
import ShaderModule.Bindless;
struct FragInput
{
    float2 inUV : TEXCOORD0;
}
[[vk::push_constant]] ConstantBuffer<CompHandles> ps;
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUnifom[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> Arr_lightUniform[];
[[vk::binding(0, 0)]] ConstantBuffer<RenderSetting> Arr_graphicSetting[];
[[vk::binding(2, 0)]] Sampler2D textures[];

[shader("fragment")]
float4 main(FragInput input) : SV_Target
{
    //Resource
    float4 posWS = textures[ps.position].Sample(input.inUV);
    float4 normal = normalize(textures[ps.normal].Sample(input.inUV));
    float4 baseColor = textures[ps.baseColor].Sample(input.inUV);
    float4 metallicRoughness = textures[ps.mr].Sample(input.inUV);

    LightCameraUniform lightUniform = Arr_lightUniform[ps.lightUniform];
    GlobalUniform globalUnifom = Arr_globalUnifom[ps.globalUniform];
    RenderSetting graphicSetting = Arr_graphicSetting[ps.renderSettingUniform];
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
        shadow = CalculateShadow(textures[ps.shadowMap], 
            mul(globalUnifom.lightProjMat, globalUnifom.lightViewMat),
             posWS,
             lp.softShadowType,
             light.lightSize,
             graphicSetting.shadowSetting);
    }
    return float4(finalColor*shadow,1.0);
    // return testValues;
    //return float4(0.0f);
    //return float4(shadow);
}