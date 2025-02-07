import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
[[vk::binding(3, 0)]] RWTexture2D<float4> outputImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<RTUniform> rtUniform_Arr[];

[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;
[[vk::binding(1, 0)]] StructuredBuffer<GeometryNode> node_Arr[];

[shader("raygeneration")]
void main()
{
    
    RTUniform rtU = rtUniform_Arr[handles.rtUniform];
    RaytracingAccelerationStructure tlas2 = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> outputImg = outputImg_Arr[handles.outputImg];

    uint2 pixelCoord = DispatchRaysIndex().xy;
    //uint seed = ((DispatchRaysIndex().y * DispatchRaysDimensions().x) + DispatchRaysIndex().x) ^ (i * 0x1234567);
    //float r1 = GetRandomNumber(seed);
    //float r2 = GetRandomNumber(seed);

    //float2 jitter = float2(r1, r2); // 确保 Random(seed) 返回0~1的随机数
    //float2 pixelCenter = float2(pixelCoord) + float2(0.5, 0.5) + jitter - 0.5;
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
    payload.maxBounceTimes = 2;
    payload.curDepth = 0;
    payload.radiance = float3(0);

    int spp = 64;
    float3 color = (0,0,0);
    for (int i = 0; i < spp; i++)
    {
        payload.curSPP = i + 1;
        payload.curDepth = 0;
        payload.radiance = float3(0);
        TraceRay(tlas2, rayFlag, 0XFF, 0, 0, 0, ray, payload);
        color += payload.radiance;
    }
    color /= spp;

    outputImg[pixelCoord] = float4(color, 1.0);

}