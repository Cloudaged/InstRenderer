import ShaderModule.BasicFunc;
import ShaderModule.PBR;
struct FragInput
{
    float2 inUV : TEXCOORD0;
}

struct Light
{
    float4 position;
    float4 toward;
    float4 color;
    int type;
    float intensity;
    float range;
};

struct LightUniform
{
    int size;
    Light light[16];
}
[[vk::binding(0, 0)]] Sampler2D inputPosition;
[[vk::binding(1, 0)]] Sampler2D inputNormal;
[[vk::binding(2, 0)]] Sampler2D inputAlbedo;
[[vk::binding(3, 0)]] Sampler2D inputMR;

[[vk::binding(1, 1)]] ConstantBuffer<LightUniform> lightUniform;

[shader("fragment")]
float4 main(FragInput input) : SV_Target
{
    float4 posWS = inputPosition.Sample(input.inUV);
    float4 normal = normalize(inputNormal.Sample(input.inUV));
    float4 baseColor = inputAlbedo.Sample(input.inUV);
    float4 metallicRoughness = inputMR.Sample(input.inUV);

    float3 F0 = GetF0(baseColor.xyz, metallicRoughness.x);

    float lambert = 0;
    for (int i = 0; i < lightUniform.size; i++)
    {
        Light light = lightUniform.light[i];
        lambert = max(0.1, dot(normal.xyz, normalize(light.toward.xyz)));
    }
    return baseColor;
}