import ShaderModule.RayTracing;

[shader("closesthit")]
void main(inout MyPayload payload)
{
    payload = MyPayload(float3(1, 0, 0));
}