import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::push_constant]] ConstantBuffer<DDGIIrradianceVolumeHandle> handles;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 coords = dispatchThreadID.xy;
    int leftPixel = IRRADIANCE_VOLUME_SIZE;
    int rightPixel = IRRADIANCE_VOLUME_SIZE - 1;
    RWTexture2D<float4> raddianceMap = rwImg_Arr[handles.radianceMap];
    RWTexture2D<float4> irradianceVolume = rwImg_Arr[handles.irradianceVolume];

    int2 coordsInVolume = coords % leftPixel;


    bool isBorder = coordsInVolume.x ==0||coordsInVolume.y==0||
    coordsInVolume.x ==(IRRADIANCE_VOLUME_SIZE-1)||coordsInVolume.y ==(IRRADIANCE_VOLUME_SIZE-1);

    int probeIndex = GetProbeIndex(coords, IRRADIANCE_VOLUME_SIZE);
    Probe probe = probe_Arr[handles.probeArea].Load(probeIndex);
    float4 result = float4(0);    
    if (true)
    {
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

            float3 raddiance = sampleValue.xyz;
            result += float4(raddiance * weight, weight);
        }
    }
    if (result.w > 0.0001)
    {
        result /= result.w;
    }
    irradianceVolume[coords] = float4(result);
}