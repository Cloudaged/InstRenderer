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

float3 Uncharted2Tonemap(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}


[Shader("fragment")]
FragmentOutput main(FragmentInput input)
{
    FragmentOutput output;
    float3 color = texture[handles.lighted].Sample(input.uv).xyz;
    //float3 mapped = hdrColor / (hdrColor + float3(1.0));

    color = Uncharted2Tonemap(color * 1);
    color = color * (1.0f / Uncharted2Tonemap(float3(11.2f)));

    output.presentColor = float4(color,1.0f);


    return output;
} 