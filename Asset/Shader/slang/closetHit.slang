import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<Material> materialA_Arr[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

RayQuery rayQuery;

[shader("closesthit")]
void main(inout MyPayload payload)
{
    StructuredBuffer<Material> matBuffer = materialA_Arr[handles.materials];
    Material mat = matBuffer.Load(InstanceID());
    
    payload = MyPayload(float3(mat.baseColor, 0, 0));
}