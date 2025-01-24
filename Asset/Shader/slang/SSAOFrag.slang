import ShaderModule.Types;
import ShaderModule.Bindless;

struct FragmentInput
{
    float2 uv : TEXCOORD0;
}

[[vk::push_constant]] ConstantBuffer<SSAOHandles> handles;
[[vk::binding(0, 0)]] ConstantBuffer<SSAOKernel> Arr_kernelUniform[];
[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniform> Arr_globalUniform[];

[[vk::binding(2, 0)]] Sampler2D textures[];

#define SSAO_RADIUS 50

[[shader("fragment")]]
float4 main(FragmentInput input):SV_Target
{
    float3 vPos = textures[handles.position].Sample(input.uv).xyz;
    float3 vNormal = textures[handles.normal].Sample(input.uv).xyz;
    SSAOKernel kernelU = Arr_kernelUniform[handles.kernels];
    GlobalUniform globalU = Arr_globalUniform[handles.global];
    int3 texDim;
    textures[handles.position].GetDimensions(0, texDim.x, texDim.y, texDim.z);
    int3 rotationDim;
    textures[handles.rotation].GetDimensions(0, texDim.x, texDim.y, texDim.z);
    float2 rotationMapUV = float2(float(texDim.x) / float(rotationDim.x), float(texDim.y) / float(rotationDim.y)) * input.uv;

    float3 randomVec = textures[handles.rotation].Sample(input.uv).xyz;

    float3 tangent = normalize(randomVec - vNormal * dot(randomVec, vNormal));

    float3 bitangent = cross(tangent, vNormal);
    float3x3 TBN = float3x3(tangent, bitangent, vNormal);
    TBN = transpose(TBN);

    float occ = 0.0f;
    float bias = -0.50f;
    int times = 0;
    for (int i = 0; i < 32; i++)
    {
        float3 samplePos = mul(TBN,kernelU.kernels[i].xyz);
        samplePos = vPos + samplePos * SSAO_RADIUS;

        float4 offset = float4(samplePos, 1.0);
        offset = mul(globalU.projMat,offset);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5;

        float sampleDepth = textures[handles.position].Sample(offset.xy).z;

        float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(vPos.z - sampleDepth));
        if (sampleDepth != 0)
        {
            occ += (sampleDepth >= samplePos.z + 0.025 ? 1.0f : 0.0f) * rangeCheck;
        }
    }
    occ = 1.0 - (occ / 32.0f);
    return float4(occ);
}
