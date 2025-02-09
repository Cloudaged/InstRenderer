import ShaderModule.Types;
import ShaderModule.Bindless;
import ShaderModule.DDGI;
struct VertInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TEXCOORD0;
    float4 bitangent : TEXCOORD1;
    uint instanceID : SV_InstanceID;
}

struct VertOutput
{
    float4 posSV : SV_Position;
}

[[vk::binding(1, 0)]] StructuredBuffer<Probe> probeAreaUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform_Arr[];
[[vk::push_constant]] ConstantBuffer<DDGIVisualProbeHandle> handles;

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;

    GlobalUniform globalU = globalUniform_Arr[handles.globalUniform];
    StructuredBuffer<Probe> probesArea = probeAreaUniform_Arr[handles.probeArea];
    Probe probe = probesArea.Load(input.instanceID);
    float3 pos = input.position.xyz + probe.position.xyz;
    output.posSV = mul(globalU.projMat, mul(globalU.viewMat, float4(pos,1.0)));
    return output;
}
