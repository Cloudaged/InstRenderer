import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;
[[vk::binding(1, 0)]] StructuredBuffer<GeometryNode> node_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(0, 0)]] ConstantBuffer<RTUniform> rtUniform_Arr[];

[shader("closesthit")]
void main(inout MyPayload payload,Attributes att)
{
    StructuredBuffer<GeometryNode> nodeBuffer = node_Arr[handles.node];
    RTUniform rtU = rtUniform_Arr[handles.rtUniform];

    float2 dims = float2(DispatchRaysDimensions().xy);
    GeometryNode node = nodeBuffer.Load(InstanceID());
    Face face = GetFaceData(node, PrimitiveIndex());
    Pixel pixel = GetPixelData(face, att.att);
    Camera cam;
    cam.dir = WorldRayDirection();
    cam.up = rtU.camUp.xyz;
    cam.right = rtU.camRight.xyz;
    UVDiff uvDiff = GetUVDifferentials(face, cam, RayTCurrent(), dims);
    float4 baseColor = textures[node.baseColor].SampleGrad(pixel.uv,uvDiff.duvdx,uvDiff.duvdy);
    //float4 baseColor = textures[node.baseColor].Sample(pixel.uv);
    payload.hitValue = float3(baseColor.xyz);
}