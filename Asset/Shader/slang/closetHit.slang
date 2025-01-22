import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] ConstantBuffer<MaterialArr> materialA_Arr;
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;

[shader("closesthit")]
void main(inout MyPayload payload)
{
    Material g = materialA_Arr;
    //MaterialArr matarr = materialA_Arr[handles.materials];
    Material mat = matarr[InstanceID];
    payload = MyPayload(float3(1, 0, 0));
}