import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
[[vk::binding(3, 0)]] RWTexture2D<float4> outputImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform_Arr[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;


[shader("raygeneration")]
void main()
{
    GlobalUniform globalU = globalUniform_Arr[handles.globalUniform];
    RaytracingAccelerationStructure tlas = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> outputImg = outputImg_Arr[handles.outputImg];
    float4x4 viewInv = transpose(globalU.viewMat);
    float4x4 projInv = transpose(globalU.projMat);

    uint2 pixelCoord = DispatchRaysIndex().xy;
    uint2 inUV = pixelCoord / DispatchRaysDimensions().xy;
    float2 d = inUV * 2.0 - 1.0;

    RayDesc ray;
    ray.Origin = mul(viewInv, float4(0, 0, 0, 1)).xyz;
    float4 target = mul(projInv, float4(d.x, d.y, 1, 1));
    ray.Direction = mul(viewInv, float4(normalize(target.xyz), 0)).xyz;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;

    uint rayFlag = RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;
    MyPayload payload;
    payload.hitValue = float3(0, 1.0, 0);

    TraceRay(tlas, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);

    outputImg[pixelCoord] = float4(payload.hitValue,0.0);
}