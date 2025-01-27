import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
[[vk::push_constant]] ConstantBuffer<RayTracingHandles> handles;
[[vk::binding(1, 0)]] StructuredBuffer<GeometryNode> node_Arr[];
[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(0, 0)]] ConstantBuffer<RTUniform> rtUniform_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> Arr_lightUniform[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];

[shader("closesthit")]
void main(inout MyPayload payload,Attributes att)
{
    StructuredBuffer<GeometryNode> nodeBuffer = node_Arr[handles.node];
    LightCameraUniform lightUniform = Arr_lightUniform[handles.lightUniform];
    RaytracingAccelerationStructure tlas2 = tlasUniform_Arr[handles.tlas];

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
    float4 baseColor = textures[node.baseColor].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    payload.hitValue = float3(baseColor.xyz);

    Light light = lightUniform.light[0];
    float3 curPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    // float3 dir = normalize(light.position.xyz - curPos);
    RayDesc ray;
    ray.Origin = curPos;
    ray.Direction = -normalize(light.toward.xyz);
    ray.TMin = 0.001;
    ray.TMax = 10000;
    uint flags = RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    payload.isInShadow = true;
    TraceRay(tlas2, flags, 0XFF, 0, 0, 1, ray, payload);
    if (payload.isInShadow)
    {
        payload.hitValue *= 0;
    }
    
}