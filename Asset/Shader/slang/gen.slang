import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
[[vk::binding(3, 0)]] RWTexture2D<float4> outputImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> lightCameraUniform_Arr[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

[shader("raygeneration")]
void main()
{
    GlobalUniform globalU = globalUniform_Arr[handles.globalUniform];
    RaytracingAccelerationStructure tlas2 = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> outputImg = outputImg_Arr[handles.outputImg];
    LightCameraUniform lightCamU = lightCameraUniform_Arr[handles.lightUniform];
    float4x4 viewInv = transpose(globalU.viewMat);
    float4x4 projInv = transpose(globalU.projMat);

    uint2 pixelCoord = DispatchRaysIndex().xy ;
    float2 pixelCenter = float2(pixelCoord) + float2(0.5,0.5);
    float2 inUV = float2(pixelCenter) / float2(DispatchRaysDimensions().xy);
    float2 d = inUV * 2.0 - 1.0;

    // RayDesc ray;
    // ray.Origin = mul(viewInv, float4(0, 0, 0, 1)).xyz;
    // float4 target = mul(projInv, float4(d.x, d.y, 1, 1));
    // float3 dir = mul(viewInv, float4(target.xyz, 1)).xyz;
    // ray.Direction = normalize(dir);
    // ray.TMin = 0.001;
    // ray.TMax = 10000.0;
    RayDesc ray;
    ray.Origin = float3(d.x, 1.0f - d.y, 1.0f);
    ray.Direction = float3(0.0f, 0.0f,-1.0f);
    ray.TMin = 0.0;
    ray.TMax = 10000.0;

    uint rayFlag = RAY_FLAG_FORCE_OPAQUE;
    MyPayload payload;
    //payload.hitValue = float3(0, 1.0, 0);

    TraceRay(tlas2, rayFlag, 0XFF, 0, 0, 0, ray, payload);

    outputImg[pixelCoord] = float4(payload.hitValue,1.0);
}