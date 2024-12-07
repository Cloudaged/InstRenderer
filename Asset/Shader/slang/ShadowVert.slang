import ShaderModule.Types;
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

[[vk::push_constant]] ConstantBuffer<PushConstantData> pushConstantData;
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform; 

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    output.positionSV = mul(globalUniform.lightProjMat,mul(globalUniform.lightViewMat, mul(pushConstantData.modelMat,input.Position)));
    return output;
}