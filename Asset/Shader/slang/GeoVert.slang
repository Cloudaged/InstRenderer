import ShaderModule.Bindless;
import ShaderModule.Types;
struct VertInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TEXCOORD0;
    float4 bitangent : TEXCOORD1;
}

struct VertOutput
{
    float4 posSV : SV_Position;
    float3 posWS:TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normalWS : TEXCOORD2;
    float3 bitangentWS : TEXCOORD3;
    float3 tangentWS : TEXCOORD4;
}

[[vk::push_constant]] ConstantBuffer<GeoHandles> handles;

[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform[];

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    output.posWS = mul(handles.modelMat, input.position).xyz;
    output.posSV = mul(globalUniform[handles.global].projMat, mul(globalUniform[handles.global].viewMat, float4(output.posWS, 1.0)));
    output.uv = input.uv;

    output.tangentWS = normalize(mul(handles.modelMat, float4(input.tangent.xyz, 0.0)).xyz);
    output.bitangentWS = normalize(mul(handles.modelMat, float4(input.bitangent.xyz, 0.0)).xyz);
    output.normalWS = normalize(mul(handles.modelMat, float4(input.normal.xyz,0.0)).xyz);
    // float3 T = normalize(input.tangent.xyz);
    // float3 B = normalize(input.bitangent.xyz);
    // float3 N = normalize(input.normal.xyz);
    return output;
}