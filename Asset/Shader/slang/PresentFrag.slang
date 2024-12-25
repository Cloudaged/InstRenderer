import ShaderModule.Bindless;

struct FragmentInput
{
    float2 uv : TEXCOORD0;
}

struct FragmentOutput
{
    float4 presentColor : SV_Target;
}

[[vk::push_constant]] ConstantBuffer<PresentHandle> handles;
[[vk::binding(2, 0)]] Sampler2D texture[];

[Shader("fragment")]
FragmentOutput main(FragmentInput input)
{
    FragmentOutput output;
    output.presentColor = texture[handles.lighted].Sample(input.uv);
    return output;
} 