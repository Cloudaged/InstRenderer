struct FragInput
{
    float3 inUVW : TEXCOORD0;
    float2 uv2D : TEXCOORD1;
}

struct FragOutput
{
    float4 color : SV_Target;
    float depth : SV_Depth;
}

[[vk::binding(0, 1)]] SamplerCube cubeMap;

[shader("fragment")]
FragOutput main(FragInput input):SV_Target
{
    FragOutput output;
    output.color = cubeMap.Sample(input.inUVW);
    output.depth = 1.0f;
    return output;
}