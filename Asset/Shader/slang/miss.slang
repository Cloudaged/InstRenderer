import ShaderModule.RayTracing;
[shader("miss")]
void main(inout MyPayload payload)
{
    payload = MyPayload(float3(0, 0.5, 0.5));
}