import ShaderModule.Types;

struct VertInput
{
    int index:SV_VertexID;
}

struct VertOutput
{
    float4 position : SV_Position;
    float2 outUV : TEXCOORD0;
}

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;

    output.outUV = float2((input.index << 1) & 2, input.index & 2);
    output.position = float4(output.outUV * 2.0f - 1.0f, 0.0f, 1.0f);
    return output;
}