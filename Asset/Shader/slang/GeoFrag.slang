struct FragInput
{
    float3 posWS : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normalWS : TEXCOORD2;
    float3x3 tbn : TEXCOORD3;
}

struct Slots
{
    int albedo;
    int normal;
    int mr;
}

struct FragOutput
{
    float4 outPosition:SV_TARGET0;
    float4 outNormal:SV_TARGET1;
    float4 outBaseColor:SV_TARGET2;
    float4 outMR:SV_TARGET3;
}

[[vk::binding(0, 1)]] ConstantBuffer<Slots> slots;
[[vk::binding(1, 1)]] Sampler2D baseColor;
[[vk::binding(2, 1)]] Sampler2D normal;
[[vk::binding(3, 1)]] Sampler2D mr;

[shader("fragment")]
FragOutput main(FragInput input)
{
    FragOutput output;
    output.outPosition = float4(input.posWS, 1.0);

    if (slots.albedo == 1)
    {
        output.outBaseColor = baseColor.Sample(input.uv);
    }
    else
    {
        output.outBaseColor = float4(0, 1, 1, 0);
    }

    if (slots.normal == 1)
    {
        float3 normalColor = normal.Sample(input.uv).xyz;
        normalColor = pow(normalColor, float3(1 / 2.2));
        normalColor = normalize(normalColor * 2.0 - 1);
        normalColor = normalize(mul(input.tbn,normalColor));
        output.outNormal = float4(normalColor, 0.0);
    }
    else
    {
        output.outNormal = float4(input.normalWS, 0.0);
    }

    if (slots.mr == 1)
    {
        output.outMR = mr.Sample(input.uv);
    }
    else
    {
        output.outMR = float4(0.0, 0.0, 0.0, 0.0);
    }
    return output;
}