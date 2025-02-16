import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::push_constant]] ConstantBuffer<IndirectLightHandle> handles;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 coords = dispatchThreadID.xy;
    Sampler2D irradianceVolume = textures[handles.irradianceVolume];
    Sampler2D depthVolume = textures[handles.depthVolume];
    RWTexture2D<float4> indirectLight = rwImg_Arr[handles.indirectLight];
    Sampler2D baseColorSampler = textures[handles.baseColor];
    int2 extent;
    baseColorSampler.GetDimensions(extent.x, extent.y);
    float2 uv = (float2)coords / (float2)extent;
    StructuredBuffer<Probe> probeBuffer = probe_Arr[handles.probeArea];

    float4 baseColor = baseColorSampler.Sample(uv);
    float4 normal = textures[handles.normal].Sample(uv);
    float4 position = textures[handles.position].Sample(uv);

    float3 cameraPos;
    float3 view = normalize(cameraPos - position.xyz);
    float minDistance = 1.0f;
    int3 baseGrid = GetGridIndice(position.xyz, handles.probeBasePos.xyz, handles.probeSpacing.xyz);
    float3 baseProbePosWS = handles.probeBasePos.xyz + baseGrid * handles.probeSpacing.xyz;
    float3 sum = 0;
    for (int z = 0; z < 2; z++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int x = 0; x < 2; x++)
            {
                int3 offset = int3(x, y, z);
                int3 probeCoord = clamp(baseGrid + offset, int3(0), int3(PROBE_AREA_SIZE - 1));
                float3 curProbePosWS = handles.probeBasePos.xyz + probeCoord * handles.probeSpacing.xyz;
                Probe probe = probeBuffer.Load(GetProbeArrayIndex1D(probeCoord));
                float3 sampleDir = normalize(position.xyz - probe.position.xyz);
                float2 uv = GetVolumeUV(sampleDir, probeCoord);
                sum += irradianceVolume.SampleLevel(uv,0).xyz;
            }
        }
    }

    float3 irradiance = sum/8.0f;
    indirectLight[coords] = float4(irradiance, 1.0);
}