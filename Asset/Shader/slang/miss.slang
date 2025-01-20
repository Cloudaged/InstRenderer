import ShaderModule.RayTracing;
[shader("miss")]
void main(inout MyPayload payload)
{
    payload.hitValue = float3(0, 0, 0.5);
}