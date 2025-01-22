import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
[[vk::binding(3, 0)]] RWTexture2D<float4> outputImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

[shader("raygeneration")]
void main()
{
    RaytracingAccelerationStructure tlas2 = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> outputImg = outputImg_Arr[handles.outputImg];
    // uint2 pixelCoord = DispatchRaysIndex().xy ;
    // float2 pixelCenter = float2(pixelCoord) + float2(0.5,0.5);
    // float2 inUV = float2(pixelCenter) / float2(DispatchRaysDimensions().xy);
    // float2 d = inUV * 2.0 - 1.0;

    // // RayDesc ray;
    // // ray.Origin = mul(viewInv, float4(0, 0, 0, 1)).xyz;
    // // float4 target = mul(projInv, float4(d.x, d.y, 1, 1));
    // // float3 dir = mul(viewInv, float4(target.xyz, 1)).xyz;
    // // ray.Direction = normalize(dir);
    // // ray.TMin = 0.001;
    // // ray.TMax = 10000.0;
    // RayDesc ray;
    // ray.Origin = float3(d.x, d.y, 100.0f);
    // ray.Direction = float3(0.0f, 0.0f,-1.0f);
    // ray.TMin = 0.0;
    // ray.TMax = 10000.0;

    // uint rayFlag = RAY_FLAG_FORCE_OPAQUE;
    // MyPayload payload;
    // //payload.hitValue = float3(0, 1.0, 0);

    uint3 LaunchID = DispatchRaysIndex();
    uint3 LaunchSize = DispatchRaysDimensions();

    const float2 pixelCenter = float2(LaunchID.xy) + float2(0.5, 0.5);
    const float2 inUV = pixelCenter / float2(LaunchSize.xy);
    float2 d = inUV * 2.0 - 1.0;

    RayDesc rayDesc;
    // rayDesc.Origin = mul(cam.viewInverse, float4(0,0,0,1)).xyz;
    rayDesc.Origin = float3(d.x, d.y, 1.0);
    rayDesc.Direction = float3(0, 0, -1);
    // rayDesc.Direction = mul(cam.viewInverse, float4(normalize(target.xyz), 0)).xyz;
    rayDesc.TMin = 0.001;
    rayDesc.TMax = 10000.0;

    MyPayload payload;
    TraceRay(tlas2, RAY_FLAG_FORCE_OPAQUE, 0xff, 0, 0, 0, rayDesc, payload);

    outputImg[int2(LaunchID.xy)] = float4(inUV,0.0, 0.0);

    // TraceRay(tlas2, rayFlag, 0XFF, 0, 0, 0, ray, payload);

    // outputImg[pixelCoord] = float4(payload.hitValue,1.0);
}