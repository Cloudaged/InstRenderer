#version 450

layout (set = 0, binding = 0)  uniform sampler2D inputPosition;
layout (set = 0, binding = 1)  uniform sampler2D inputNormal;
layout (set = 0, binding = 2)  uniform sampler2D inputAlbedo;

layout (location = 0) in vec2 inUV;


layout(location =0) out vec4 outColor;
void main()
{

    vec3 Pos = texture(inputPosition,inUV).xyz;
    vec4 normal = texture(inputNormal,inUV);
    vec4 baseColor = texture(inputAlbedo,inUV);

    outColor = baseColor;

}