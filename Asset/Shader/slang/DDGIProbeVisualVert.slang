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
    float4 normalWS : TEXCOORD0;
    int instanceID : TEXCOORD1;
}
[[vk::binding(3, 0)]] readonly RWTexture2D<float4> rwImg_Arr[];

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

    RWTexture2D<float4> probeOffsetMap = rwImg_Arr[handles.probeOffset];
    int2 sampleCoord = GetProbeOffsetMapUV(input.instanceID);
    float3 offset = probeOffsetMap[sampleCoord].xyz;

    float3 pos = input.position.xyz + probe.position.xyz+offset;
    output.posSV = mul(globalU.projMat, mul(globalU.viewMat, float4(pos, 1.0)));
    output.normalWS = input.normal;
    output.instanceID = input.instanceID;
    return output;
}
