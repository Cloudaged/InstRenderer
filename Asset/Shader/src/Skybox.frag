#version 450

layout (set=2,binding = 0) uniform samplerCube samplerCubeMap;
layout(set=1,binding =0) sampler2D lighted;

layout(location = 0) in vec3 inUVW;

layout(location = 0) out vec4 outColor;


void main()
{
    gl_FragDepth = 1.0;
    outColor = texture(samplerCubeMap, inUVW);
    //outColor = vec4(1.0,0.0,0.0,1.0);
}