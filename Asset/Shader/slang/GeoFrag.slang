import ShaderModule.Bindless;
import ShaderModule.Types;
struct FragInput
{
    float4 positionSV : SV_Position;
    float3 posWS : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normalWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float3 tangentWS : TEXCOORD4;
    float3 posVS : TEXCOORD5;
    float3 normalVS : TEXCOORD6;
}

struct FragOutput
{
    float4 outPosition:SV_TARGET0;
    float4 outNormal:SV_TARGET1;
    float4 outBaseColor:SV_TARGET2;
    float4 outMR : SV_TARGET3;
    float4 outPosVS : SV_TARGET4;
    float4 outNormalVS : SV_TARGET5;
}
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];
[[vk::push_constant]] ConstantBuffer<GeoHandles> handles;
[[vk::binding(2, 0)]] Sampler2D textures[];

float LinearDepth(float depth,float nearPlane,float farPlane)
{
    return -(2.0 * nearPlane * farPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
}

[shader("fragment")]
FragOutput main(FragInput input)
{
    GlobalUniform globalU = Arr_globalUniform[handles.global];
    FragOutput output;
    output.outPosition = float4(input.posWS, 1.0);
    output.outPosVS = float4(input.posVS, LinearDepth(input.positionSV.z,globalU.nearPlane,globalU.farPlane));
    int basecolorIndex = handles.material.baseColor;
    int normalIndex = handles.material.normal;
    int mrIndex = handles.material.metallicRoughness;

    if (basecolorIndex != 0)
    {
        // output.outBaseColor = pow(baseColor.Sample(input.uv),float4(2.2));
        output.outBaseColor = textures[basecolorIndex].Sample(input.uv);
    }
    else
    {
        output.outBaseColor = float4(1, 0, 0, 1);
    }

    if (normalIndex != 0)
    {
        float3 normalColor = textures[normalIndex].Sample(input.uv).xyz;
        normalColor = pow(normalColor, float3(1 / 2.2));
        normalColor = normalColor * 2.0 - 1;
        float3x3 tbn = float3x3(input.tangentWS, input.bitangentWS, input.normalWS);
        tbn = transpose(tbn);
        normalColor = normalize(mul(tbn, normalColor));
        output.outNormal = float4(normalColor, 0.0);
        output.outNormalVS = mul(globalU.viewMat, float4(normalColor, 0.0));    
    }
    else
    {
        output.outNormal = float4(input.normalWS, 0.0);
        output.outNormalVS = mul(globalU.viewMat, float4(input.normalWS, 0.0));
    }

    if (mrIndex != 0)
    {
        //float4 mrColor = mr.Sample(input.uv);
        float4 mrColor = pow(textures[mrIndex].Sample(input.uv),1/2.2);
        output.outMR = mrColor;
    }
    else
    {
        output.outMR = float4(0.0, 1.0, 0.0, 0.0);
    }
    return output;
}