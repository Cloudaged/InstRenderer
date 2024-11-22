#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(set=2,binding=0) uniform ModelBuffer
{
    mat4 modelMat;
}modelBuffer;

layout(set=0,binding=0) uniform GlobalUniform
{
    mat4 viewMat;
    mat4 projMat;
}globalUniform;

layout(push_constant) uniform PushConstantsData
{
    mat4 modelMat;
}pushConstants;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location =3 ) out mat3  tbn;
void main()
{
    outWorldPos = (pushConstants.modelMat*vec4(inPosition,1.0)).xyz;

    gl_Position = globalUniform.projMat*globalUniform.viewMat*vec4(outWorldPos,1.0);
    outUV = uv;

    vec3 T = normalize(vec3(pushConstants.modelMat*vec4(tangent,0.0)));
    vec3 B = normalize(vec3(pushConstants.modelMat*vec4(bitangent,0.0)));
    vec3 N = normalize(vec3(pushConstants.modelMat*vec4(normal,0.0)));
    tbn = mat3(T,B,N);

    outNormal = (pushConstants.modelMat*vec4(normal,0.0)).xyz;
}