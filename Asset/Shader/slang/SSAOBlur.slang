import ShaderModule.Bindless;

[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(3, 0)]] RWTexture2D rwTextures[];
[[vk::push_constant]] ConstantBuffer<SSAOBlurHandle> handles;
[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    int2 pixel = dispatchThreadID.xy;
    Sampler2D ssao = textures[handles.ssao];
    int2 textureSize;
    ssao.GetDimensions(textureSize.x, textureSize.y);
    float2 uv = (float2)pixel / (float2)textureSize;
    RWTexture2D ssaoBlur = rwTextures[handles.ssaoBlur];

    int n = 0;
    float result = 0;
    float2 texelSize = 1 / (float2)textureSize;
    for (int x = -handles.xBlurRange; x < handles.xBlurRange; x++)
    {
        float2 offset = float2(float(x), 0) * texelSize;
        result += ssao.Sample(uv + offset).r;
        n++;
    }
    for (int y = -handles.yBlurRange; y < handles.yBlurRange; y++)
    {
        int2 offset = int2(0, y);
        result += ssaoBlur.Load(pixel + offset).r;
        n++;
    }
    result /= n;
    ssaoBlur.Store(pixel, float4(result));
}