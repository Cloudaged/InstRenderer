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
        
    }

    // Sample previous offset
    float3 currentOffset = probeOffsetMap[sampleCoord].xyz;

    float3 fullOffset = 1000000;
    float3 maxProbeOffsetScale = 0.4;
    float3 offsetLimit = maxProbeOffsetScale * handles.spacing.xyz;

    float _ProbeMinFrontfaceDistance = 10;
        // check
    bool insideGeometry = (float(backfaceCount) / RAYS_PER_PROBE) > 0.25f;
    if (insideGeometry && (closestBackfaceIndex != -1))
    {
        // if inside geometry
        // float scaleFactor = 2.0f;
        // for (int i = 1; i < 100; i++)
        // {
        //     if (!all(abs(fullOffset) <= offsetLimit))
        //     {
        //         fullOffset = currentOffset + FibonacciSphere(closestBackfaceIndex, RAYS_PER_PROBE) * (scaleFactor * (i - 0.01));
        //     }
        // }

        float3 closestBackVec = FibonacciSphere(closestBackfaceIndex, RAYS_PER_PROBE);
        fullOffset = currentOffset + closestBackVec * (closestFrontfaceDistance + _ProbeMinFrontfaceDistance * 0.5);
    } else if (closestFrontfaceDistance < _ProbeMinFrontfaceDistance)
    {
        float3 closestFrontfaceVec = FibonacciSphere(closestFrontfaceIndex, RAYS_PER_PROBE);
        float3 farthestFrontfaceVec = FibonacciSphere(farthestFrontfaceIndex, RAYS_PER_PROBE);

        if (dot(closestFrontfaceVec, farthestFrontfaceVec) < 0.5f)
        {
            float3 farthestDir = farthestFrontfaceVec * min(1.f, farthestFrontfaceDistance);
            fullOffset = currentOffset + farthestDir;
        }
    } else if (closestFrontfaceDistance > _ProbeMinFrontfaceDistance)
    {
        float moveBackMargin = min(closestFrontfaceDistance - _ProbeMinFrontfaceDistance, length(currentOffset));
        float3 moveBackDirection = normalize(-currentOffset);
        fullOffset = currentOffset + (moveBackMargin * moveBackDirection);
    }

    // if (all(abs(fullOffset) < offsetLimit))
    // {
    //     probeOffsetMap[sampleCoord] = float4(fullOffset, 1.0);
    // }

    float3 normalizedOffset = fullOffset / handles.spacing.xyz;
    if (dot(normalizedOffset, normalizedOffset) < 0.2025f) // 0.45 * 0.45 == 0.2025
    {
        currentOffset = fullOffset;
    }

    probeOffsetMap[sampleCoord] = float4(currentOffset, 1.0);
}