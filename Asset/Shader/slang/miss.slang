import ShaderModule.RayTracing;
import ShaderModule.Bindless;
[[vk::binding(2, 0)]] SamplerCube cubeMap[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

[shader("miss")]
void main(inout MyPayload payload)
{

    payload.radiance = clamp(cubeMap[handles.skyboxTex].SampleLevel(WorldRayDirection(),0).xyz,0,1);

}