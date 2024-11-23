[[vk::binding(0,0)]] Sampler2D lightAttachment;

struct FragmentInput
{
    float2 uv : TEXCOORD0;
}

struct FragmentOutput
{
    float4 presentColor : SV_Target;
}
[Shader("fragment")]
FragmentOutput main(FragmentInput input)
{
    FragmentOutput output;
    output.presentColor = lightAttachment.Sample(input.uv);
    return output;
} 