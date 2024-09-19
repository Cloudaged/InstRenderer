#version 450
layout(location =0) out vec4 outColor;

layout(set=1,binding=0) uniform sampler2D tex1;

layout(location =0) in vec2 inUV;

void main()
{
    outColor = texture(tex1,inUV);
}