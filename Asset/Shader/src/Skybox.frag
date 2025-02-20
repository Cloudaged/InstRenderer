#version 450

layout (set=1,binding = 0) uniform samplerCube samplerCubeMap;
layout(location = 0) in vec3 inUVW;
layout(location = 1) in vec2 uv2D;
layout(location = 0) out vec4 outColor;


void main()
{
    gl_FragDepth = 1.0;
    outColor = texture(samplerCubeMap, inUVW);

    //outColor = vec4(1.0,0.0,0.0,1.0);
}