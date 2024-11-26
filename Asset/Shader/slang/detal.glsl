#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 11 0
layout(binding = 1)
uniform sampler2D inputNormal_0;


#line 12
layout(binding = 2)
uniform sampler2D inputAlbedo_0;


#line 13
layout(binding = 3)
uniform sampler2D inputMR_0;


#line 19 1
struct Light_std140_0
{
    vec4 position_0;
    vec4 toward_0;
    vec4 color_0;
    int type_0;
    float intensity_0;
    float range_0;
};


#line 19
struct _Array_std140_Light16_0
{
    Light_std140_0  data_0[16];
};


#line 19
struct LightCameraUniform_std140_0
{
    vec4 cameraPos_0;
    vec4 cameraToward_0;
    int lightCount_0;
    _Array_std140_Light16_0 light_0;
};


#line 15 0
layout(binding = 1, set = 1)
layout(std140) uniform block_LightCameraUniform_std140_0
{
    vec4 cameraPos_0;
    vec4 cameraToward_0;
    int lightCount_0;
    _Array_std140_Light16_0 light_0;
}lightUniform_0;

#line 4 2
vec3 GetF0_0(vec3 albedo_0, float metallic_0)
{

    return mix(vec3(0.03999999910593033, 0.03999999910593033, 0.03999999910593033), albedo_0, vec3(metallic_0));
}


vec3 GetDiffuseBRDF_0(vec3 albedo_1)
{
    return albedo_1 * 0.31830990314483643;
}


#line 12 1
struct Light_0
{
    vec4 position_0;
    vec4 toward_0;
    vec4 color_0;
    int type_0;
    float intensity_0;
    float range_0;
};


#line 12
Light_0 unpackStorage_0(Light_std140_0 _S1)
{

#line 12
    Light_0 _S2 = { _S1.position_0, _S1.toward_0, _S1.color_0, _S1.type_0, _S1.intensity_0, _S1.range_0 };

#line 12
    return _S2;
}


#line 16 2
vec3 F_Schlick_0(vec3 F0_0, float VoH_0)
{
    return F0_0 + (1.0 - F0_0) * pow(VoH_0, 5.0);
}

float D_GGX_0(float roughness_0, float NoH_0)
{
    float alpha_0 = roughness_0 * roughness_0;
    float alpha2_0 = alpha_0 * alpha_0;
    float denom_0 = NoH_0 * NoH_0 * (alpha2_0 - 1.0) + 1.0;
    return alpha2_0 / (3.14159250259399414 * denom_0 * denom_0);
}

float G_SchlickGGX_0(float roughness_1, float NoV_0)
{
    float k_0 = pow(roughness_1 + 1.0, 2.0) / 8.0;

    return NoV_0 / (NoV_0 * (1.0 - k_0) + k_0);
}

vec3 GetSpecularBRDF_0(vec3 N_0, vec3 L_0, vec3 V_0, float roughness_2, vec3 F0_1)
{
    vec3 H_0 = normalize(V_0 + L_0);

#line 45
    float NoL_0 = dot(N_0, L_0);
    float NoV_1 = dot(N_0, V_0);

#line 54
    return F_Schlick_0(F0_1, dot(V_0, H_0)) * D_GGX_0(roughness_2, dot(N_0, H_0)) * (G_SchlickGGX_0(roughness_2, NoV_1) * G_SchlickGGX_0(roughness_2, NoL_0)) / (4.0 * NoL_0 * NoV_1);
}


#line 54
layout(location = 0)
out vec4 entryPointParam_main_0;


#line 54
layout(location = 0)
in vec2 input_inUV_0;


#line 18 0
void main()
{

    vec4 _S3 = normalize((texture((inputNormal_0), (input_inUV_0))));

    vec4 metallicRoughness_0 = (texture((inputMR_0), (input_inUV_0)));


    float _S4 = metallicRoughness_0.y;

    vec3 _S5 = (texture((inputAlbedo_0), (input_inUV_0))).xyz;

#line 28
    vec3 F0_2 = GetF0_0(_S5, metallicRoughness_0.x);

    vec3 _S6 = vec3(1.0) - F0_2;

    vec3 _S7 = GetDiffuseBRDF_0(_S5);

    vec3 _S8 = lightUniform_0.cameraToward_0.xyz;

    const vec3 _S9 = vec3(0.0);

#line 36
    int i_0 = 0;

#line 36
    vec3 finalColor_0 = _S9;
    for(;;)
    {

#line 37
        if(i_0 < lightUniform_0.lightCount_0)
        {
        }
        else
        {

#line 37
            break;
        }

#line 37
        Light_0 _S10 = unpackStorage_0(lightUniform_0.light_0.data_0[i_0]);



        vec3 _S11 = _S3.xyz;

#line 41
        vec3 _S12 = _S10.toward_0.xyz;

        vec3 finalColor_1 = finalColor_0 + _S10.intensity_0 * dot(_S11, _S12) * (_S6 * _S7 + F0_2 * GetSpecularBRDF_0(_S11, _S12, _S8, _S4, F0_2));

#line 37
        i_0 = i_0 + 1;

#line 37
        finalColor_0 = finalColor_1;

#line 37
    }

#line 37
    entryPointParam_main_0 = vec4(finalColor_0, 1.0);

#line 37
    return;
}

