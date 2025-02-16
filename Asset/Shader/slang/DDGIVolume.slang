import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::push_constant]] ConstantBuffer<DDGIIrradianceVolumeHandle> handles;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::binding(3, 0)]] RWTexture2D<float2> depthImg_Arr[];
const int kReadTable[6] = {5, 3, 1, -1, -3, -5};

[shader("compute")]
[numthreads(6, 6, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 coords = dispatchThreadID.xy;
    RWTexture2D<float4> raddianceMap = rwImg_Arr[handles.radianceMap];
    RWTexture2D<float4> irradianceVolume = rwImg_Arr[handles.irradianceVolume];
    RWTexture2D<float2> depthVolume = depthImg_Arr[handles.depthVolume];
    int2 coordsInVolume = coords % IRRADIANCE_VOLUME_SIZE;
    int2 volumeCoord = coords / IRRADIANCE_VOLUME_SIZE;
    int probeIndex = GetVolumeProbeArrayIndex(coords, IRRADIANCE_VOLUME_SIZE);
    Probe probe = probe_Arr[handles.probeArea].Load(probeIndex);
    float4 irradianceResult = float4(0);
    float2 depthResult = 0;

    int backface = 0;
    int maxBackface = int(RAYS_PER_PROBE * 0.1f);
    for (int i = 0; i < RAYS_PER_PROBE; i++)
    {
        int2 uv = int2(probeIndex, i);
        float3 rayDir = FibonacciSphere(i, RAYS_PER_PROBE);
        float3 texelDir = OctDecode(coordsInVolume, IRRADIANCE_VOLUME_SIZE);
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
    irradianceVolume[coords] = float4(irradianceResult);
    depthVolume[coords] = float2(depthResult);

    //change corner
    GroupMemoryBarrierWithGroupSync();
    int2 source = int2(0);
    bool isCorner = (coordsInVolume.x == 0 || coordsInVolume.y == 0) &&
                    (coordsInVolume.x == (IRRADIANCE_VOLUME_SIZE - 1) || coordsInVolume.y == (IRRADIANCE_VOLUME_SIZE - 1));

    bool isRow = coordsInVolume.x > 0 && coordsInVolume.x < (IRRADIANCE_VOLUME_SIZE - 1);
    if (isCorner)
    {
        source.x += coordsInVolume.x == 0 ? (IRRADIANCE_VOLUME_SIZE - 1) : (-IRRADIANCE_VOLUME_SIZE + 1); 
        source.y += coordsInVolume.y == 0 ? (IRRADIANCE_VOLUME_SIZE - 1) : (-IRRADIANCE_VOLUME_SIZE + 1);
    }else if(isRow)
    {
        source.x += IRRADIANCE_VOLUME_SIZE-1-coordsInVolume.x;
        source.y += coordsInVolume.y+((coordsInVolume.y>0)?-1:1);
    }
    else 
    {
        source.x += coordsInVolume.x+((coordsInVolume.x>0)?-1:1);
        source.y += IRRADIANCE_VOLUME_SIZE-1-coordsInVolume.y;
    }
    source = source + IRRADIANCE_VOLUME_SIZE*volumeCoord;
    irradianceVolume[coords] = irradianceVolume[source];
    depthVolume[coords] = depthVolume[source];
}