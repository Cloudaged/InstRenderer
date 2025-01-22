module Bindless;
import Types;

//#define GEN_BUFFER_DEFINE(type) [[vk::binding(0, 0)]] ConstantBuffer<type> type_value[];

//GEN_BUFFER_DEFINE(GlobalUniform)

#define Handle public int
public struct Material
{
   Handle baseColor;
   Handle normal;
   Handle metallicRoughness;
}

public struct GeoHandles
{
    Handle global;
    public Material material;
    public float4x4 modelMat;    
}

public struct ShadowMapHandles
{
    public float4x4 modelMat;
    Handle global;
}

public struct CascadedShadowHandles
{
    public float4x4 modelMat;
    Handle csmData;
}

public struct SkyboxHandles
{
    Handle skyboxTex;
    Handle global;
}

public struct CompHandles
{
    Handle position;
    Handle normal;
    Handle baseColor;
    Handle mr;
    Handle shadowMap;
    Handle csmUniform;
    Handle lightUniform;
    Handle globalUniform;
    Handle renderSettingUniform;
}

public struct RayTracingHandles
{
    Handle outputImg;
    Handle tlas;
}

public struct PresentHandle
{
    Handle lighted;
    Handle rtImg;
}



