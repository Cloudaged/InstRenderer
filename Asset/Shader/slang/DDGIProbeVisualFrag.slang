import ShaderModule.Bindless;
struct FragInput
{
    float4 color : COLOR0;
};

struct FragOutput
{
    float4 color : SV_Target;
}

[shader("fragment")]
FragOutput main(FragInput input)
{
    FragOutput output;
    // output.color = float4(input.color);
    output.color = float4(1.0, 0, 0, 1);
    return output;
}