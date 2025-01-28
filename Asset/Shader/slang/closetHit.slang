import ShaderModule.RayTracing;
import ShaderModule.Types;
import ShaderModule.Bindless;
import ShaderModule.Light;
import ShaderModule.PBR;
import ShaderModule.BasicFunc;
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
    float3 curPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    float3 view = normalize(lightUniform.cameraPos.xyz - curPos.xyz);

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
    // Shading
    float4 baseColor = textures[node.baseColor].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    float4 normalColor = textures[node.normal].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    normalColor = pow(normalColor, float4(1 / 2.2));
    normalColor = normalColor * 2.0 - 1.0;
    float3 normal = UnpackNormalMap(normalColor.xyz, pixel.tangent, pixel.bitangent, pixel.normal);
    float4 metallicRoughness = textures[node.metallicRoughness].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    metallicRoughness = pow(metallicRoughness, float4(1/2.2));
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;
    Light light = lightUniform.light[0];
    LightProperty lp = GetLightProperty(light, curPos.xyz);
    // diffuse
    float3 F0 = GetF0(baseColor.xyz, metallic);
    float3 ks = F0;
    float3 kd = float3(1.0) - F0;
    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);
    //specular
    float3 specularBRDF = GetSpecularBRDF(normal, lp.lightDir, view, roughness, F0);
    float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
     float3 finalColor = lp.intensity * dot(normal, lp.lightDir.xyz) * finalBRDF;
     // float3 finalColor = specularBRDF;
     payload.hitValue = float3(finalColor.xyz);

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
        payload.hitValue *= 1;
    }
    
}