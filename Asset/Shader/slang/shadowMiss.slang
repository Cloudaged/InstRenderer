import ShaderModule.RayTracing;
[shader("miss")]
void main(inout MyPayload payload)
{
    payload.visibility = 1.0;
}