import ShaderModule.Types;
struct VertInput
{
    float4 Position;
}

struct VertOutput
{
    float4 positionSV : SV_POSITION;
}

[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform; 

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    output.positionSV = mul(globalUniform.lightMat, input.Position);
    return output;
}