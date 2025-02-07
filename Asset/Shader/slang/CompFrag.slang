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
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> Arr_lightUniform[];
[[vk::binding(0, 0)]] ConstantBuffer<RenderSetting> Arr_graphicSetting[];
[[vk::binding(0, 0)]] ConstantBuffer<CSMUniform> Arr_csmUniform[];

[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(2, 0)]] Sampler2DArray textureArr[];

[shader("fragment")]
float4 main(FragInput input) : SV_Target
{
    //Resource
    float4 posWS = textures[ps.position].Sample(input.inUV);
    float4 normal = normalize(textures[ps.normal].Sample(input.inUV));
    float4 baseColor = textures[ps.baseColor].Sample(input.inUV);
    float4 metallicRoughness = textures[ps.mr].Sample(input.inUV);
    LightCameraUniform lightUniform = Arr_lightUniform[ps.lightUniform];
    GlobalUniform globalUnifom = Arr_globalUniform[ps.globalUniform];
    RenderSetting graphicSetting = Arr_graphicSetting[ps.renderSettingUniform];
    CSMUniform csmU = Arr_csmUniform[ps.csmUniform];
    float ssao = textures[ps.ssao].Sample(input.inUV).r;
    int csmIndex = 0;
    int blendIndex = 0;
    float blendValue = 0.0;
    float t = 0.0;


    for (int i = 0; i < CASCADED_COUNT - 1; ++i)
    {
        float di = distance(posWS.xyz, csmU.cascadeSplits[i].xyz);
        float ri = csmU.cascadeSplits[i].w;
        if (di > ri+0.1)
        {
            csmIndex = i + 1;
        }
        else
        {
            if (di > ri - csmU.radiusBias)
            {
                blendIndex = min(csmIndex + 1, CASCADED_COUNT - 1);
                blendValue = (di -(ri-csmU.radiusBias)) / csmU.radiusBias;
            }
            break;
        } 
    }

    // if (graphicSetting.shadowSetting.showCascaded == -1)
    // {
    //     return baseColor + ShowCascade(csmIndex);
    // } else if (graphicSetting.shadowSetting.showCascaded!=-2)
    // {
    //     return float4(textureArr[ps.shadowMap].Sample(float3(input.inUV, graphicSetting.shadowSetting.showCascaded)).r);
    // }

    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    float3 F0 = GetF0(baseColor.xyz, metallic);


    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);

    float3 view = normalize(lightUniform.cameraPos.xyz-posWS.xyz);

    float3 finalColor = float3(0.0f);
    float shadow = 0;
    for (int i = 0; i < lightUniform.lightCount; i++)
    {
        Light light = lightUniform.light[i];
        LightProperty lp = GetLightProperty(light, posWS.xyz);

        float3 H = normalize(lp.lightDir + view);
        float VoH = clamp(dot(view, H), 0.0001, 1);
        float3 F = F_Schlick(F0, VoH);
        float3 ks = F0;
        float3 kd = float3(1.0) - F0;

        float3 specularBRDF = GetSpecularBRDF(normal.xyz, lp.lightDir, view, roughness, F);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
        finalColor += lp.intensity * saturate(dot(normal.xyz, lp.lightDir.xyz)) * finalBRDF;

        // finalColor = specularBRDF;
        shadow = CSM(textureArr[ps.shadowMap], csmIndex,blendIndex,blendValue,
            csmU,
             posWS,
             lp.softShadowType,
             light.lightSize,
             graphicSetting.shadowSetting);
    }

    float3 hdrColor = finalColor;
    return float4(hdrColor,1.0);
    // return testValues;
    //return float4(0.0f);
    //return float4(shadow);
}