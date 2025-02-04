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
    float4 baseColor = (0, 0, 0, 0);
    float3 normal = pixel.normal;
    float4 metallicRoughness = float4(0, 0, 0, 0);
    if (node.baseColor != 0)
    {
        // output.outBaseColor = pow(baseColor.Sample(input.uv),float4(2.2));
        baseColor = textures[node.baseColor].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
    }
    if (node.normal != 0)
    {
        float4 normalColor = textures[node.normal].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
        normalColor = pow(normalColor, float4(1 / 2.2));
        normalColor = normalColor * 2.0 - 1.0;
        normal = UnpackNormalMap(normalColor.xyz, pixel.tangent, pixel.bitangent, pixel.normal);
    }
    if (node.metallicRoughness != 0)
    {
        metallicRoughness = textures[node.metallicRoughness].SampleGrad(pixel.uv, uvDiff.duvdx, uvDiff.duvdy);
        metallicRoughness = pow(metallicRoughness, float4(1 / 2.2));
    }
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;
    Light light = lightUniform.light[0];
    LightProperty lp = GetLightProperty(light, curPos.xyz);

    // Secondary bounce
    payload.curDepth++;
    uint seed = ((DispatchRaysIndex().y * DispatchRaysDimensions().x) + DispatchRaysIndex().x)+payload.curSPP * 1664525;
    float cosineP,brdfP;
    // float3 secDir = MultipleSample(normal, -WorldRayDirection(),roughness, cosineP,brdfP,seed);
    // float3 secDir = GetRandomCosineDirectionOnHemisphere(normal, seed);
    float3 secDir = GetBrdfPDFDir(normal, -WorldRayDirection(), roughness, seed);
    if (payload.curDepth < payload.maxBounceTimes)
    {
        RayDesc secRay;
        secRay.Origin = curPos;
        secRay.Direction = secDir;
        secRay.TMin = 0.001;
        secRay.TMax = 10000;
        payload.visibility = 0.0;
        TraceRay(tlas2, RAY_FLAG_FORCE_OPAQUE, 0XFF, 0, 0, 0, secRay, payload);
    }
    //BRDF
    // diffuse
    float3 F0 = GetF0(baseColor.xyz, metallic);
    
    float3 diffuseBRDF = GetDiffuseBRDF(baseColor.xyz);

    float3 directColor = (0,0,0);
    //direct Light
    {
        float3 halfDir = normalize(lp.lightDir + (-WorldRayDirection()));
        float VoH = clamp(dot(-WorldRayDirection(), halfDir), 0.0001, 1);
        float3 F = F_Schlick(F0, VoH);
        float3 ks = F;
        float3 kd = float3(1.0) - F0;
        kd *= (1.0 - metallic);

        // specular
        float3 specularBRDF = GetSpecularBRDF(normal, lp.lightDir, -WorldRayDirection(), roughness, F0);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
        directColor = lp.intensity *float3(1,1,1)* max(dot(normal, lp.lightDir.xyz),0.0) * finalBRDF;
    }
    float3 indirectColor= (0,0,0);
    // indirect light
    if (payload.curDepth < payload.maxBounceTimes)
    {
        float3 halfDir = normalize(secDir + (-WorldRayDirection()));
        float a = max(roughness, 1e-1);
        float VoH = clamp(dot(-WorldRayDirection(), halfDir), 0.0001, 1);
        float HoN = clamp(dot(halfDir, normal), 0.0001, 1);
        float a2 = a * a;
        float num = a2;
        float den = PI * pow(((a2 - 1) * pow(HoN, 2) + 1), 2);
        float halfPdf = num*HoN / den;
        float brdfPdf = halfPdf / (4 * VoH);

        float3 F = F_Schlick(F0, VoH);
        float3 ks = F;
        float3 kd = float3(1.0) - F0;
        kd *= (1.0 - metallic);
        // specular
        float3 specularBRDF = GetSpecularBRDF(normal, secDir, -WorldRayDirection(), roughness, F0);
        float3 finalBRDF = kd * diffuseBRDF + ks * specularBRDF;
        float3 brdfColor = float3(payload.radiance) * max(dot(normal, secDir), 0.0) * finalBRDF/brdfPdf;
        indirectColor = brdfColor;
        // indirectColor = float3(payload.radiance) * max(dot(normal, secDir), 0.0) * finalBRDF;
        //indirectColor = roughness;
    }
    
    //Visibility
    RayDesc ray;
    ray.Origin = curPos;
    ray.Direction = -normalize(light.toward.xyz);
    ray.TMin = 0.001;
    ray.TMax = 10000;
    uint flags = RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    payload.visibility = 0;
    TraceRay(tlas2, flags, 0XFF, 0, 0, 1, ray, payload);

    if (payload.curDepth == 2)
    {
        payload.radiance = directColor * payload.visibility + indirectColor;
    } else
    {
        payload.radiance = indirectColor;
    }
    //payload.radiance = directColor * payload.visibility + indirectColor;
    payload.curDepth--;
}