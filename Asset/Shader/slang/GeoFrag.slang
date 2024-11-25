struct FragInput
{
    float3 posWS : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normalWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float3 tangentWS : TEXCOORD4;
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
         //output.outBaseColor = pow(baseColor.Sample(input.uv),float4(2.2));
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
        normalColor = normalColor * 2.0 - 1;
        float3x3 tbn = float3x3(input.tangentWS, input.bitangentWS, input.normalWS);
        tbn = transpose(tbn);
        normalColor = normalize(mul(tbn, normalColor));
        output.outNormal = float4(normalColor, 0.0);
    }
    else
    {
        output.outNormal = float4(input.normalWS, 0.0);
    }

    if (slots.mr == 1)
    {
        //float4 mrColor = mr.Sample(input.uv);
        float4 mrColor = pow(mr.Sample(input.uv),1/2.2);
        output.outMR = mrColor;
    }
    else
    {
        output.outMR = float4(0.0, 0.0, 0.0, 0.0);
    }
    return output;
}