import ShaderModule.Bindless;
import ShaderModule.Types;
import ShaderModule.RayTracing;
import ShaderModule.DDGI;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];

[[vk::push_constant]] ConstantBuffer<DDGIRadianceHandle> handles;

[shader("raygeneration")]
void main()
{

    RaytracingAccelerationStructure tlas = tlasUniform_Arr[handles.tlas];
    RWTexture2D<float4> raddianceMap = rwImg_Arr[handles.radianceMap];
    RWTexture2D<float4> probeOffset = rwImg_Arr[handles.probeOffset];

    uint2 pixelCoord = DispatchRaysIndex().xy;
    float2 pixelCenter = float2(pixelCoord) + float2(0.5, 0.5);
    int probeCount = PROBE_AREA_SIZE * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
    StructuredBuffer<Probe> probeBuffer = probe_Arr[handles.probesArea];
    uint rayFlag = RAY_FLAG_FORCE_OPAQUE;
    
    DDGIPayload payload;
    payload.radiance = float3(0);
    payload.distance = 0.0f;
    payload.curDepth = 0;
    Probe p = probeBuffer.Load(pixelCoord.x);
    float3 offset = probeOffset.Load(GetProbeOffsetMapUV(pixelCoord.x)).xyz;
    RayDesc ray;
    ray.Origin = p.position.xyz+offset;
    ray.Direction = FibonacciSphere(pixelCoord.y,RAYS_PER_PROBE);
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    TraceRay(tlas, rayFlag, 0XFF, 0, 0, 0, ray, payload);
    raddianceMap[pixelCoord] = float4(payload.radiance,payload.distance);
    
    
}
