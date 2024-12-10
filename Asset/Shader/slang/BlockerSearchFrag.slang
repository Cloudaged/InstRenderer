import ShaderModule.Shadow;
import ShaderModule.Types;
struct FragInput
{
    float2 inUV : TEXCOORD0;
    float4x4 lightMat : TEXCOORD1;
}
[[vk::binding(0,0)]] Sampler2D position;
[[vk::binding(1,0)]] Sampler2D shadowMap;

[shader("fragment")]
float4 main(FragInput input)
{

    //float avgDepth = GetAvgBlockerDepth(shadowMap, input.lightMat, position.Sample(input.inUV));
    return float4(0.0, 0.0,0.0,0.0);
}