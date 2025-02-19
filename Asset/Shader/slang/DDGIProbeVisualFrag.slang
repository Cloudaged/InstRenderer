import ShaderModule.Bindless;
import ShaderModule.DDGI;
import ShaderModule.Types;

struct FragInput
{
    float4 normalWS : TEXCOORD0;
    int instanceID : TEXCOORD1;
};

struct FragOutput
{
    float4 color : SV_Target;
}
[[vk::push_constant]] ConstantBuffer<DDGIVisualProbeHandle> handles;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probeAreaUniform_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];

[shader("fragment")]
FragOutput main(FragInput input)
{
    FragOutput output;
    // output.color = float4(input.color);
    int3 grid = GetProbeCoords3D(input.instanceID);

    float2 uv = GetVolumeUV(normalize(input.normalWS.xyz), grid);

    float3 color = textures[handles.irradianceVolume].Sample(uv).xyz;
    output.color = float4(color,1);
    return output;
}