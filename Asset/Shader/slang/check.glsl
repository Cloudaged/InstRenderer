#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 13 0
layout(binding = 0)
uniform sampler2D inputPosition_0;


#line 14
layout(binding = 1)
uniform sampler2D inputNormal_0;


#line 15
layout(binding = 2)
uniform sampler2D inputAlbedo_0;


#line 16
layout(binding = 3)
uniform sampler2D inputMR_0;


#line 21 1
struct Light_std140_0
{
    vec4 position_0;
    vec4 toward_0;
    vec4 color_0;
    int type_0;
    float intensity_0;
    float range_0;
};


#line 21
struct _Array_std140_Light16_0
{
    Light_std140_0  data_0[16];
};


#line 21
struct LightCameraUniform_std140_0
{
    vec4 cameraPos_0;
    vec4 cameraToward_0;
    int lightCount_0;
    _Array_std140_Light16_0 light_0;
};


#line 19 0
layout(binding = 1, set = 1)
layout(std140) uniform block_LightCameraUniform_std140_0
{
    vec4 cameraPos_0;
    vec4 cameraToward_0;
    int lightCount_0;
    _Array_std140_Light16_0 light_0;
}lightUniform_0;

#line 17
layout(binding = 4)
uniform sampler2D shadowMap_0;


#line 5 2
struct _Array_std140_vectorx3Cfloatx2C2x3E16_0
{
    vec2  data_1[16];
};


#line 5
struct GlobalParams_std140_0
{
    int PCF_SAMPLE_COUNTS_0;
    _Array_std140_vectorx3Cfloatx2C2x3E16_0 poissonDisk_0;
};


#line 5
layout(binding = 5)
layout(std140) uniform block_GlobalParams_std140_0
{
    int PCF_SAMPLE_COUNTS_0;
    _Array_std140_vectorx3Cfloatx2C2x3E16_0 poissonDisk_0;
}globalParams_0;

#line 5
void unpackStorage_0(_Array_std140_vectorx3Cfloatx2C2x3E16_0 _S1, out vec2  _S2[16])
{

#line 5
    _S2[0] = _S1.data_1[0];

#line 5
    _S2[1] = _S1.data_1[1];

#line 5
    _S2[2] = _S1.data_1[2];

#line 5
    _S2[3] = _S1.data_1[3];

#line 5
    _S2[4] = _S1.data_1[4];

#line 5
    _S2[5] = _S1.data_1[5];

#line 5
    _S2[6] = _S1.data_1[6];

#line 5
    _S2[7] = _S1.data_1[7];

#line 5
    _S2[8] = _S1.data_1[8];

#line 5
    _S2[9] = _S1.data_1[9];

#line 5
    _S2[10] = _S1.data_1[10];

#line 5
    _S2[11] = _S1.data_1[11];

#line 5
    _S2[12] = _S1.data_1[12];

#line 5
    _S2[13] = _S1.data_1[13];

#line 5
    _S2[14] = _S1.data_1[14];

#line 5
    _S2[15] = _S1.data_1[15];

#line 5
    return;
}


#line 4 3
vec3 GetF0_0(vec3 albedo_0, float metallic_0)
{

    return mix(vec3(0.03999999910593033, 0.03999999910593033, 0.03999999910593033), albedo_0, vec3(metallic_0));
}


vec3 GetDiffuseBRDF_0(vec3 albedo_1)
{
    return albedo_1 * 0.31830990314483643;
}


#line 14 1
struct Light_0
{
    vec4 position_0;
    vec4 toward_0;
    vec4 color_0;
    int type_0;
    float intensity_0;
    float range_0;
};


#line 14
Light_0 unpackStorage_1(Light_std140_0 _S3)
{

#line 14
    Light_0 _S4 = { _S3.position_0, _S3.toward_0, _S3.color_0, _S3.type_0, _S3.intensity_0, _S3.range_0 };

#line 14
    return _S4;
}


#line 16 3
vec3 F_Schlick_0(vec3 F0_0, float VoN_0)
{
    return F0_0 + (1.0 - F0_0) * pow(1.0 - VoN_0, 5.0);
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


    float NoL_0 = clamp(dot(N_0, L_0), 0.0, 1.0);
    float NoV_1 = clamp(dot(N_0, V_0), 0.0, 1.0);

#line 54
    return F_Schlick_0(F0_1, NoV_1) * D_GGX_0(roughness_2, clamp(dot(N_0, normalize(V_0 + L_0)), 0.0, 1.0)) * (G_SchlickGGX_0(roughness_2, NoV_1) * G_SchlickGGX_0(roughness_2, NoL_0)) / (4.0 * NoL_0 * NoV_1);
}


#line 25 2
float NormalBias_0(float biasValue_0, vec3 normal_0, vec3 lightDir_0)
{



    return biasValue_0 * max(length(cross(normal_0, lightDir_0)), 0.0);
}


#line 10762 4
layout(location = 0)
out vec4 entryPointParam_main_0;


#line 10762
layout(location = 0)
in vec2 input_inUV_0;


#line 10762
layout(row_major)
layout(location = 1)
in mat4x4 input_lightViewMat_0;


#line 10762
layout(row_major)
layout(location = 5)
in mat4x4 input_lightProjMat_0;


#line 43 0
float PCF_0(vec2 _S5, float _S6, float _S7)
{

#line 43
    int i_0 = 0;

#line 43
    float sum_0 = 0.0;

#line 36 2
    for(;;)
    {

#line 36
        if(i_0 < globalParams_0.PCF_SAMPLE_COUNTS_0)
        {
        }
        else
        {

#line 36
            break;
        }

#line 36
        vec2  _S8[16];

#line 36
        unpackStorage_0(globalParams_0.poissonDisk_0, _S8);



        if(_S6 - 0.00100000004749745 > (texture((shadowMap_0), (_S5 + _S8[i_0]))).x)
        {

#line 40
            sum_0 = sum_0 + 1.0;

#line 40
        }

#line 36
        i_0 = i_0 + 1;

#line 36
    }

#line 45
    return sum_0 / float(globalParams_0.PCF_SAMPLE_COUNTS_0);
}


#line 45
float CalculateShadow_0(mat4x4 _S9, vec4 _S10, float _S11)
{



    vec4 posInLightNDC_0 = (((_S10) * (_S9)));
    vec4 posInLightNDC_1 = posInLightNDC_0 / posInLightNDC_0.w;

#line 58
    return PCF_0(posInLightNDC_1.xy * 0.5 + 0.5, posInLightNDC_1.z, 0.00100000004749745);
}


#line 22 0
void main()
{
    vec4 posWS_0 = (texture((inputPosition_0), (input_inUV_0)));
    vec4 _S12 = normalize((texture((inputNormal_0), (input_inUV_0))));

    vec4 metallicRoughness_0 = (texture((inputMR_0), (input_inUV_0)));


    float _S13 = metallicRoughness_0.y;

    vec3 _S14 = (texture((inputAlbedo_0), (input_inUV_0))).xyz;

#line 32
    vec3 F0_2 = GetF0_0(_S14, metallicRoughness_0.z);

    vec3 _S15 = vec3(1.0) - F0_2;

    vec3 _S16 = GetDiffuseBRDF_0(_S14);

    vec3 _S17 = normalize(lightUniform_0.cameraPos_0.xyz - posWS_0.xyz);

    const vec3 _S18 = vec3(0.0);

#line 40
    float shadow_0 = 0.0;

#line 40
    int i_1 = 0;

#line 40
    vec3 finalColor_0 = _S18;


    for(;;)
    {

#line 43
        if(i_1 < lightUniform_0.lightCount_0)
        {
        }
        else
        {

#line 43
            break;
        }

#line 43
        Light_0 _S19 = unpackStorage_1(lightUniform_0.light_0.data_0[i_1]);


        vec3 lightDir_1 = - normalize(_S19.toward_0.xyz);

        vec3 _S20 = _S12.xyz;

        vec3 finalColor_1 = finalColor_0 + _S19.intensity_0 * dot(_S20, lightDir_1.xyz) * (_S15 * _S16 + F0_2 * GetSpecularBRDF_0(_S20, lightDir_1, _S17, _S13, F0_2));

#line 50
        float _S21 = CalculateShadow_0((((input_lightViewMat_0) * (input_lightProjMat_0))), posWS_0, NormalBias_0(0.00100000004749745, _S20, lightDir_1));

#line 43
        int _S22 = i_1 + 1;

#line 43
        shadow_0 = _S21;

#line 43
        i_1 = _S22;

#line 43
        finalColor_0 = finalColor_1;

#line 43
    }

#line 43
    entryPointParam_main_0 = vec4(shadow_0 * finalColor_0, 1.0);

#line 43
    return;
}

