import ShaderModule.RayTracing;

[shader("closesthit")]
void main(inout MyPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    payload.hitValue = float3(1.0, 0.0, 0.0);
}