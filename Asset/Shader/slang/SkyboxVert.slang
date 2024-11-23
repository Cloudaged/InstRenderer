struct VertInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
}

struct VertOutput
{
    float4 svPosition : SV_Position;
    float3 outUVW : TEXCOORD0;
    float2 outUV2 : TEXCOORD1;
}

struct GlobalUniform
{
    float4x4 viewMat;
    float4x4 projMat;
    float4x4 skyboxProjMat;
}
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform;

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    output.svPosition = mul(globalUniform.skyboxProjMat, mul(globalUniform.viewMat, input.position));
    output.outUV2 = input.uv;
    output.outUVW = input.position.xyz;
    return output;
}