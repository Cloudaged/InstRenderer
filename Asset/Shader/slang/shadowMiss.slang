import ShaderModule.RayTracing;
[shader("miss")]
void main(inout MyPayload payload)
{
    payload.isInShadow = false;
}