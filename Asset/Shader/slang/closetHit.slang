import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::binding(1, 0)]] StructuredBuffer<GeometryNode> node_Arr[];
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;
RayQuery rayQuery;


[shader("closesthit")]
void main(inout MyPayload payload)
{
    StructuredBuffer<GeometryNode> nodeBuffer = node_Arr[handles.materials];
    GeometryNode node = nodeBuffer.Load(InstanceID());
    VerticesArr *varr = (VerticesArr *)node.vertices;
    float a= varr.v[0].position.x;
    payload.hitValue = float3(node.baseColor, InstanceID(),a);
    //payload.hitValue = float3(1, 0, 0);
}