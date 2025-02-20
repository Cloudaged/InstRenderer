import ShaderModule.Types;
import ShaderModule.Bindless;
struct VertInput
{
    float4 Position;
    uint viewID : SV_ViewID;
}

struct VertOutput
{
    float4 positionSV : SV_POSITION;
}

[[vk::push_constant]] ConstantBuffer<ShadowMapHandles> shadowmapHandles;
//[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform;
[[vk::binding(0, 0)]] ConstantBuffer<CSMUniform> Arr_csmUniform[];

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    CSMUniform csmU = Arr_csmUniform[shadowmapHandles.csm];
    output.positionSV = mul(csmU.vpMat[input.viewID], mul(shadowmapHandles.modelMat,input.Position));
    return output;
}
