import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
[[vk::binding(3, 0)]] RWTexture2D<float4> outputImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<RTUniform> rtUniform_Arr[];

[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

[shader("raygeneration")]
void main()
{
    
    RTUniform rtU = rtUniform_Arr[handles.rtUniform];
    RaytracingAccelerationStructure tlas2 = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> outputImg = outputImg_Arr[handles.outputImg];

    uint2 pixelCoord = DispatchRaysIndex().xy ;
    float2 pixelCenter = float2(pixelCoord) + float2(0.5,0.5);
    float2 inUV = float2(pixelCenter) / float2(DispatchRaysDimensions().xy);
    float2 d = inUV * 2.0 - 1.0;

    float4x4 viewInv = rtU.invView;
    float4x4 projInv = rtU.invProj;

    RayDesc ray;
    ray.Origin = mul(viewInv, float4(0, 0, 0, 1)).xyz;
    float4 target = mul(projInv, float4(d.x, d.y, 1, 1));
    float3 dir = mul(viewInv, normalize(float4(target.xyz, 0))).xyz;
    ray.Direction = dir;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;

    uint rayFlag = RAY_FLAG_FORCE_OPAQUE;
    MyPayload payload;
    TraceRay(tlas2, rayFlag, 0XFF, 0, 0, 0, ray, payload);
    outputImg[pixelCoord] = float4(payload.hitValue, 1.0);

}