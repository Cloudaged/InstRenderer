import ShaderModule.Bindless;
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
[[vk::push_constant]] ConstantBuffer<SkyboxHandles> handles;
[[vk::binding(2, 0)]] SamplerCube cubeMap[];

[shader("fragment")]
FragOutput main(FragInput input)
{
    FragOutput output;
    output.color = cubeMap[handles.skyboxTex].Sample(input.inUVW);
    output.depth = 1.0f;
    return output;
}