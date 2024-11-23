#version 450

layout (set = 0, binding = 0)  uniform sampler2D inputPosition;
layout (set = 0, binding = 1)  uniform sampler2D inputNormal;
layout (set = 0, binding = 2)  uniform sampler2D inputAlbedo;

struct Light
{
    vec4 position;
    vec4 toward;
    vec4 color;
    int type;
    float intensity;
    float range;
};

layout(set=1,binding=1) uniform LightUniform
{
    int size;
    Light light[16];
}lightUniform;



layout (location = 0) in vec2 inUV;
layout(location =0) out vec4 outColor;
void main()
{

    vec3 Pos = texture(inputPosition,inUV).xyz;
    vec4 normal = texture(inputNormal,inUV);
    vec4 baseColor = texture(inputAlbedo,inUV);
    float lambert=0;
    for(int i=0;i<lightUniform.size;i++)
    {
        Light light = lightUniform.light[i];
        lambert=max(dot(normalize(normal.xyz), normalize(light.toward.xyz)),0.1);

    }

    outColor =  baseColor*lambert;

}