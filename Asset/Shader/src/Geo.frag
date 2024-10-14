#version 450
layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout(set =1,binding =0) uniform sampler2D basecolor;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outBaseColor;

void main()
{
    outPosition = vec4(inWorldPos,1.0);
    outNormal =  vec4(normalize(inNormal),1.0);
    outBaseColor = texture(basecolor,inUV);
}