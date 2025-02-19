import ShaderModule.DDGI;
import ShaderModule.Types;
import ShaderModule.Bindless;
import ShaderModule.Light;
import ShaderModule.PBR;
import ShaderModule.BasicFunc;
import ShaderModule.RayTracing;

[[vk::binding(1, 0)]] StructuredBuffer<GeometryNode> node_Arr[];
[[vk::binding(0, 0)]] ConstantBuffer<RTUniform> rtUniform_Arr[];

[[vk::push_constant]] ConstantBuffer<DDGIRadianceHandle> handles;
[[vk::binding(2, 0)]] Sampler2D textures[];
[[vk::binding(0, 0)]] ConstantBuffer<LightCameraUniform> Arr_lightUniform[];
[[vk::binding(4, 0)]] RaytracingAccelerationStructure tlasUniform_Arr[];

[shader("closesthit")]
void main(inout DDGIPayload payload,Attributes att)
{
    //avoid caculate back face
    if (HitKind() == HIT_KIND_TRIANGLE_BACK_FACE)
    {
        payload.distance = RayTCurrent() * -1.0;
        payload.radiance = 0;
        return;
    }

    RaytracingAccelerationStructure tlas = tlasUniform_Arr[handles.tlas];
    LightCameraUniform lightUniform = Arr_lightUniform[handles.lightData];
    StructuredBuffer<GeometryNode> nodeBuffer = node_Arr[handles.geometryNodeArray];
    GeometryNode node = nodeBuffer.Load(InstanceID());
    RTUniform rtU = rtUniform_Arr[handles.rtUniform];

    Face face = GetFaceData(node, PrimitiveIndex());
    Pixel pixel = GetPixelData(face, att.att);

    float2 dims = float2(DispatchRaysDimensions().xy);
    Camera cam;
    cam.dir = WorldRayDirection();
    cam.up = rtU.camUp.xyz;
    cam.right = rtU.camRight.xyz;

    UVDiff uvDiff = GetUVDifferentials(face, cam, RayTCurrent(), dims);
    float3 normal = pixel.normal;
    float4 metallicRoughness = float4(0, 0, 0, 0);
    float4 baseColor = float4(0.0);
    float3 curPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    Light light = lightUniform.light[0];
    LightProperty lp = GetLightProperty(light, curPos.xyz);
    float4 sunColor = SunColor(lp.lightDir);
    if (node.baseColor != 0)
    {
        // output.outBaseColor = pow(baseColor.Sample(input.uv),float4(2.2));
        baseColor = textures[node.baseColor].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    }
    else
    {
        baseColor = float4(1.0, 0, 0, 1.0);
    }
    if (node.normal != 0)
    {
        float4 normalColor = textures[node.normal].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
        normalColor = pow(normalColor, float4(1 / 2.2));
        normalColor = normalColor * 2.0 - 1.0;
        normal = UnpackNormalMap(normalColor.xyz, pixel.tangent, pixel.bitangent, pixel.normal);
    }
    else
    {
        normal = pixel.normal;
    }
    if (node.metallicRoughness != 0)
    {
        metallicRoughness = textures[node.metallicRoughness].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
        metallicRoughness = pow(metallicRoughness, float4(1 / 2.2));
    }
    else
    {
        metallicRoughness.b = 0.0;
        metallicRoughness.g = 1.0; 
    }
    
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    RayDesc ray;
    ray.Origin = curPos;
    ray.Direction = -normalize(light.toward.xyz);
    ray.TMin = 0.001;
    ray.TMax = 10000;
    uint flags = RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    payload.visibility = 0;
    TraceRay(tlas, flags, 0XFF, 0, 0, 1, ray, payload);

    float3 F0 = GetF0(baseColor.xyz, metallic);

    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);

    float3 directColor = (0, 0, 0);
    // direct Light
    {
        float3 halfDir = normalize(lp.lightDir + (-WorldRayDirection()));
        float VoH = clamp(dot(-WorldRayDirection(), halfDir), 0.0001, 1);
        float VoN = clamp(dot(-WorldRayDirection(), normal), 0.0001, 1);
        float3 F = F_Schlick(F0, VoN);
        float3 ks = F;
        float3 kd = float3(1.0) - F;
        kd *= (1.0 - metallic);

        // specular
        float3 specularBRDF = GetSpecularBRDF(normal, lp.lightDir, -WorldRayDirection(), roughness, F0);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
         directColor = lp.intensity * float3(1, 1, 1) * max(dot(normal, lp.lightDir.xyz), 0.0) * finalBRDF;
    }
    float sunIntensity = sunColor.w;
    payload.radiance = sunIntensity*sunColor.xyz*directColor.xyz*payload.visibility;
    payload.distance = RayTCurrent();
}