import ShaderModule.Types;
import ShaderModule.Bindless;
struct VertInput
{
    float4 Position;
}

struct VertOutput
{
    float4 positionSV : SV_POSITION;
}
struct PushConstantData
{
    float4x4 modelMat;
}

[[vk::push_constant]] ConstantBuffer<ShadowMapHandles> shadowmapHandles;
//[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform;
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    GlobalUniform globalU = Arr_globalUniform[shadowmapHandles.global];
    output.positionSV = mul(globalU.lightProjMat, mul(globalU.lightViewMat, mul(shadowmapHandles.modelMat,input.Position)));
    return output;
}