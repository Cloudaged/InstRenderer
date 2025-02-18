import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::push_constant]] ConstantBuffer<DDGIProbeOffsetHandle> handles;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];

[shader("compute")]
[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int probeIndex = dispatchThreadID.x;
    int probeCounts = PROBE_COUNT;
    if (probeIndex > probeCounts)
    {
        return;
    }
    int closestBackfaceIndex = -1;
    float closestBackfaceDistance = 1000000.0;

    int closestFrontfaceIndex = -1;
    float closestFrontfaceDistance = 1000000.0;

    float farthestFrontfaceDistance = 0;
    float farthestFrontfaceIndex = 0;

    int backfaceCount = 0;

    RWTexture2D<float4> radianceMap = rwImg_Arr[handles.radianceMap];
    RWTexture2D<float4> probeOffsetMap = rwImg_Arr[handles.probeOffsetMap];
    StructuredBuffer<Probe> probeBuffer = probe_Arr[handles.probeArea];
    GlobalUniform globalU = Arr_globalUniform[handles.global];

    Probe probe = probeBuffer.Load(probeIndex);
    int2 sampleCoord = GetProbeOffsetMapUV(probeIndex);

    for (int i = 0; i < RAYS_PER_PROBE; i++)
    {
        int2 rayCoord = int2(probeIndex, i);
        float distance = radianceMap.Load(rayCoord).w;
        if (distance <= 0.0)//backface
        {
            backfaceCount++;
            if ((-distance) < closestBackfaceDistance)
            {
                closestBackfaceDistance = distance;
                closestBackfaceIndex = i;
            }
        }
        else
        {
            if (distance < closestFrontfaceDistance)
            {
                closestFrontfaceDistance = distance;
                closestFrontfaceIndex = i;
            }
            else if(distance>farthestFrontfaceDistance)
            {
                farthestFrontfaceDistance = distance;
                farthestFrontfaceIndex = i;
            }
        }
        //Sample previous offset
        float3 currentOffset = 0;
        if (globalU.isFirstFrame != 0)
        {
            currentOffset = probeOffsetMap[sampleCoord].xyz;
        }
        float3 currentPos = probe.position.xyz + currentOffset;
        float3 fullOffset = float3(10000.0f);
        float3 maxProbeOffsetScale = 0.4;
        float3 cellOffsetLimit = maxProbeOffsetScale * handles.spacing.xyz;

        //check
        bool insideGeometry = (float(backfaceCount) / RAYS_PER_PROBE) > 0.25f;
        if (insideGeometry && (closestBackfaceIndex != -1))
        {
            //if inside geometry
            float3 closestBackfaceVec = closestBackfaceDistance * FibonacciSphere(closestBackfaceIndex, RAYS_PER_PROBE);
            probeOffsetMap[sampleCoord].xyz = closestBackfaceVec;            
        }

    }
}