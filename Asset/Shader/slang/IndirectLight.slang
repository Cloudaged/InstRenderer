import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
import ShaderModule.PBR;
[[vk::binding(1, 0)]] StructuredBuffer<Probe> probe_Arr[];
[[vk::push_constant]] ConstantBuffer<IndirectLightHandle> handles;
[[vk::binding(3, 0)]] RWTexture2D<float4> rwImg_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> globalUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> Arr_lightUniform[];

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 coords = dispatchThreadID.xy;
    Sampler2D irradianceVolume = textures[handles.irradianceVolume];
    Sampler2D depthVolume = textures[handles.depthVolume];
    RWTexture2D<float4> indirectLight = rwImg_Arr[handles.indirectLight];
    RWTexture2D<float4> probeOffsetMap = rwImg_Arr[handles.probeOffset];
    LightCameraUniform lightUniform = Arr_lightUniform[handles.lightUniform];

    Sampler2D baseColorSampler = textures[handles.baseColor];
    int2 extent;
    baseColorSampler.GetDimensions(extent.x, extent.y);
    float2 uv = (float2)coords / (float2)extent;
    StructuredBuffer<Probe> probeBuffer = probe_Arr[handles.probeArea];

    float4 baseColor = baseColorSampler.Sample(uv);
    float4 normal = textures[handles.normal].Sample(uv);
    float4 position = textures[handles.position].Sample(uv);

    float3 view = normalize(lightUniform.cameraPos.xyz - position.xyz);
    float minDisBetPros = min(handles.probeSpacing.x, min(handles.probeSpacing.y, handles.probeSpacing.z));
    float3 biasVector = (normal.xyz * 0.2f + view * 0.8f) * (0.75f * minDisBetPros) * 0.3;
    float minDistance = 1.0f;
    float3 biasedPos = position.xyz +biasVector;
    int3 baseGrid = GetGridIndice(biasedPos.xyz, handles.probeBasePos.xyz, handles.probeSpacing.xyz);
    float3 baseProbePosWS = handles.probeBasePos.xyz + baseGrid * handles.probeSpacing.xyz;
    float3 sum = 0;
    float sumWeight = 0;


    for (int z = 0; z < 2; z++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int x = 0; x < 2; x++)
            {
                int3 offset = int3(x, y, z);
                int3 probeCoord = clamp(baseGrid + offset, int3(0), int3(PROBE_AREA_SIZE - 1));
                Probe probe = probeBuffer.Load(GetProbeArrayIndex1D(probeCoord));

                int2 offsetSampleCoord = GetProbeOffsetMapUV(GetProbeArrayIndex1D(probeCoord));
                float4 offsetMapValue = probeOffsetMap[offsetSampleCoord].xyzw;
                if (offsetMapValue.w < 0)
                {
                    continue;
                }
                float3 offsetedProbePosition = probe.position.xyz + offsetMapValue.xyz;
                float3 distance = clamp(float3(biasedPos - baseProbePosWS) / handles.probeSpacing.xyz, float3(0.0f), float3(1.0f));
                float3 trilinear = lerp(1.0 - distance,distance, offset);
                float weight = 1.0f;
                weight *= trilinear.x * trilinear.y * trilinear.z + 0.001f;
                float3 sampleDir = normalize(offsetedProbePosition.xyz - biasedPos);
                float2 uv = GetVolumeUV(sampleDir, probeCoord);

                float2 visibility = depthVolume.SampleLevel(uv,0).xy;
                float meanDistance = visibility.x;
                float distanceToCurProbe = length(offsetedProbePosition - biasedPos);
                float chebyshevWeight = 1.0;
                if (distanceToCurProbe > meanDistance)
                {
                    float variance = abs(visibility.x * visibility.x - visibility.y);
                    float distanceDiff = distanceToCurProbe - visibility.x;
                    chebyshevWeight = variance / (variance + (distanceDiff * distanceDiff));
                    chebyshevWeight = max((chebyshevWeight * chebyshevWeight * chebyshevWeight), 0.0f);
                }
                chebyshevWeight = max(0.05f, chebyshevWeight);
                weight *= chebyshevWeight;
                weight *= max(0, dot(normal.xyz, sampleDir));                sum += weight*irradianceVolume.SampleLevel(uv,0).xyz;
                sumWeight +=weight;
            }
        }
    }

    float3 receive = sum/float3(sumWeight);

    indirectLight[coords] = float4(receive, 1.0);
}