import ShaderModule.RayTracing;

[shader("closesthit")]
void main(inout MyPayload payload)
{
    payload.hitValue = float3(1.0, 0.0, 0.0);
}