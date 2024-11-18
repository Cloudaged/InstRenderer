#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

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

void main()
{
    outWorldPos = (pushConstants.modelMat*vec4(inPosition,1.0)).xyz;

    gl_Position = globalUniform.projMat*globalUniform.viewMat*vec4(outWorldPos,1.0);
    outUV = uv;

    outNormal = (pushConstants.modelMat*vec4(normal,0.0)).xyz;
}