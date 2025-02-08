import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
import ShaderModule.Light;
import ShaderModule.PBR;
import ShaderModule.BasicFunc;
import ShaderModule.RayTracing;

[shader("closesthit")]
void main(inout DDGIPayload payload,Attributes att)
{
    payload.radiance = float3(1.0, 0.0, 0.0);
}