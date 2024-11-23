#version 450
layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;
layout(location =3) in mat3 tbn;

layout(set=1,binding =0) uniform Slots
{
    int albedo;
    int normal;
    int mr;
}slot;

layout(set =1,binding =1) uniform sampler2D basecolor;
layout(set =1,binding =2) uniform sampler2D normal;
layout(set =1,binding =3) uniform sampler2D metallicRoughness;


layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outBaseColor;
layout(location=3) out vec4 outMR;

void main()
{
    /*vec4 basecolor = texture(basecolor,inUV);
    vec4 normal = texture(normal,inUV);
    vec4 metallicRoughness = texture(metallicRoughness,inUV);*/

   /* outBaseColor = slot.albedo==1?texture(basecolor,inUV):vec4(0.0,1.0,1.0,0.0);
    outPosition = slot.mr==1?texture(metallicRoughness,inUV):vec4(0,0,0,0);
    outNormal = slot.normal==1?texture(normal,inUV):vec4(inNormal,0.0);*/

    outPosition = vec4(inWorldPos,1.0);

    if(slot.albedo==1)
    {
        outBaseColor = texture(basecolor,inUV);
    }
    else
    {
        outBaseColor = vec4(0,1,1,0);
    }

    if(slot.normal==1)
    {
        vec3 normalColor= texture(normal,inUV).xyz;
        normalColor = pow(normalColor, vec3(1/2.2));
        normalColor = normalize(normalColor*2.0-1);
        normalColor = normalize(tbn*normalColor);
        outNormal = vec4(normalColor,0.0);

    }
    else
    {
        outNormal= vec4(inNormal,0.0);
    }

    if(slot.mr==1)
    {
        outMR = texture(metallicRoughness,inUV);
    }
    else
    {
        outMR= vec4(0.0,0.0,0.0,0.0);
    }




}