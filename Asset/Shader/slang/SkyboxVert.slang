import ShaderModule.Types;
import ShaderModule.Bindless;
struct VertInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
}

struct VertOutput
{
    float4 svPosition : SV_Position;
    float3 outUVW : TEXCOORD0;
    float2 outUV2 : TEXCOORD1;
}
[[vk::push_constant]] ConstantBuffer<SkyboxHandles> handles;
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];

[shader("vertex")]
VertOutput main(VertInput input)
{
    GlobalUniform globalUniform = Arr_globalUniform[handles.global];

    VertOutput output;
    float3x3 viewMat3 = (float3x3)globalUniform.viewMat; 
    float4x4 viewMat = float4x4(
        float4(viewMat3[0], 0.0),  
        float4(viewMat3[1], 0.0),
        float4(viewMat3[2], 0.0),  
        float4(0.0, 0.0, 0.0, 1.0));    
    output.svPosition = mul(globalUniform.skyboxProjMat, mul(viewMat, input.position));
    output.outUV2 = input.uv;
    output.outUVW = input.position.xyz;
    return output;
}