import ShaderModule.DDGI;
import ShaderModule.Bindless;
[[vk::binding(2, 0)]] SamplerCube cubeMap[];
[[vk::push_constant]] ConstantBuffer<DDGIRadianceHandle> handles;

[shader("miss")]
void main(inout DDGIPayload payload)
{

    payload.radiance = clamp(cubeMap[handles.skyboxTex].SampleLevel(WorldRayDirection(), 0).xyz, 0, 1);
    payload.distance = 10000;
}