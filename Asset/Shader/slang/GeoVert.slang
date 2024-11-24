
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
    float3x3 tbn:TEXCOORD3;
}

struct PushConstantData
{
    float4x4 modelMat;
}


[[vk::push_constant]] ConstantBuffer<PushConstantData> pushConstantData;

[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform;

[shader("vertex")]
VertOutput main(VertInput input)
{
    VertOutput output;
    output.posWS = mul(input.position, pushConstantData.modelMat).xyz;
    output.posSV = mul(globalUniform.projMat, mul(globalUniform.viewMat, float4(output.posWS, 1.0)));
    output.normalWS = mul(pushConstantData.modelMat, input.normal).xyz;
    output.uv = input.uv;

    float3 T = normalize(mul(pushConstantData.modelMat, float4(input.tangent.xyz, 0.0)).xyz);
     float3 B = normalize(mul(pushConstantData.modelMat, float4(input.bitangent.xyz,0.0)).xyz);
     float3 N = normalize(mul(pushConstantData.modelMat, float4(input.normal.xyz,0.0)).xyz);
    // float3 T = normalize(input.tangent.xyz);
    // float3 B = normalize(input.bitangent.xyz);
    // float3 N = normalize(input.normal.xyz);
    output.tbn = transpose(float3x3(T, B, N));
    return output;
}