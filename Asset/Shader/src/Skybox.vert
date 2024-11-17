#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(set=0,binding=0) uniform GlobalUniform
{
    mat4 viewMat;
    mat4 projMat;
    mat4 skyboxProjMat;
}globalUniform;

layout(location = 0) out vec3 outUVW;
layout(location = 1) out vec2 uv2D;

void main()
{
    mat4 viewMat = mat4(mat3(globalUniform.viewMat));

    gl_Position = globalUniform.skyboxProjMat*viewMat*vec4(inPosition.x,inPosition.y,inPosition.z, 1.0);

    outUVW = inPosition;
    uv2D = uv;
}