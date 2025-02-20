import ShaderModule.DDGI;
import ShaderModule.Bindless;
[[vk::binding(2, 0)]] SamplerCube cubeMap[];
[[vk::push_constant]] ConstantBuffer<DDGIRadianceHandle> handles;

[shader("miss")]
void main(inout DDGIPayload payload)
{
    payload.visibility = 1.0;
}