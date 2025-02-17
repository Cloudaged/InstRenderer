import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::push_constant]] ConstantBuffer<DDGIIrradianceVolumeHandle> handles;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::binding(3, 0)]] RWTexture2D<float2> depthImg_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];

[shader("compute")]
[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 coords = dispatchThreadID.xy;
    RWTexture2D<float4> raddianceMap = rwImg_Arr[handles.radianceMap];
    RWTexture2D<float4> irradianceVolume = rwImg_Arr[handles.irradianceVolume];
    RWTexture2D<float2> depthVolume = depthImg_Arr[handles.depthVolume];

    int2 coordsInVolume = coords % (IRRADIANCE_VOLUME_SIZE);//int coord in volume
    int2 volumeCoord = coords / (IRRADIANCE_VOLUME_SIZE);//Volume id in volume map
    int probeIndex = GetVolumeProbeArrayIndex(coords, IRRADIANCE_VOLUME_SIZE);
    Probe probe = probe_Arr[handles.probeArea].Load(probeIndex);
    float4 irradianceResult = float4(0);
    float2 depthResult = 0;

    bool isBorder = coordsInVolume.x == 0 || coordsInVolume.y == 0 ||
                    (coordsInVolume.x == (IRRADIANCE_VOLUME_SIZE - 1) || coordsInVolume.y == (IRRADIANCE_VOLUME_SIZE - 1));
    if(!isBorder)
    {
        int backface = 0;
        int maxBackface = int(RAYS_PER_PROBE * 0.1f);
        
        for (int i = 0; i < RAYS_PER_PROBE; i++)
        {
            int2 uv = int2(probeIndex, i);
            float3 rayDir = FibonacciSphere(i, RAYS_PER_PROBE);
            float3 texelDir = OctDecode(NormalizedOctCoords(coordsInVolume));
            float weight = max(0.0, dot(texelDir, rayDir));
            float4 sampleValue = raddianceMap.Load(uv);
            float distance = sampleValue.w;

            if (distance < 0.0f)
            {
                continue;
            }

            float3 raddiance = sampleValue.xyz;
            irradianceResult += float4(raddiance * weight, weight);
            depthResult += float2(distance * weight, distance * distance*weight); 
        }

        if (irradianceResult.w > 0.0001)
        {
            irradianceResult /= irradianceResult.w;
            depthResult /= irradianceResult.w;
        }

        float2 uv = float2(coords) / float2(PROBE_AREA_SIZE * PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE, PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE);
        float4 lastIrradiance = irradianceVolume.Load(coords);

        // irradianceVolume[coords] = lerp(irradianceResult, lastIrradiance,0.5);
        irradianceVolume[coords] = irradianceResult;
        depthVolume[coords] = float2(depthResult);
    }
    else
    {
        GroupMemoryBarrierWithGroupSync();
        int2 dst= 0;
        bool isCorner = (coordsInVolume.x == 0 || coordsInVolume.x == (IRRADIANCE_VOLUME_SIZE - 1)) &&
                        (coordsInVolume.y == 0 || coordsInVolume.y == (IRRADIANCE_VOLUME_SIZE - 1));

        bool isRow = coordsInVolume.x > 0 && coordsInVolume.x < (IRRADIANCE_VOLUME_SIZE - 1);
        if (isCorner)
        {
            dst.x = coordsInVolume.x == 0 ? (VOLUME_INTERIOR_SIZE) : 1;
            dst.y = coordsInVolume.y == 0 ? (VOLUME_INTERIOR_SIZE) : 1;
        }else if(isRow)
        {
            dst.x = IRRADIANCE_VOLUME_SIZE - 1 - coordsInVolume.x;
            dst.y = ((coordsInVolume.y == 0) ? 1 : VOLUME_INTERIOR_SIZE);
        }
        else
        {
            dst.x = ((coordsInVolume.x == 0) ? 1 : VOLUME_INTERIOR_SIZE);
            dst.y = IRRADIANCE_VOLUME_SIZE-1-coordsInVolume.y;
        }
        dst =dst + volumeCoord*IRRADIANCE_VOLUME_SIZE;

        irradianceVolume[coords] = irradianceVolume[dst];
        depthVolume[coords] = depthVolume[dst];

    }

}