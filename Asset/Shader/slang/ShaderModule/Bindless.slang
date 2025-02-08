module Bindless;
import Types;

//#define GEN_BUFFER_DEFINE(type) [[vk::binding(0, 0)]] ConstantBuffer<type> type_value[];

//GEN_BUFFER_DEFINE(GlobalUniform)

#define Handle public int

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

public struct SSAOHandles
{
    Handle position;
    Handle normal;
    Handle kernels;
    Handle rotation;
    Handle global;
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
    Handle ssao;
}

public struct RayTracingHandles
{
    Handle outputImg;
    Handle tlas;
    Handle rtUniform;
    Handle node;
    Handle lightUniform;
    Handle skyboxTex;
}

public struct PresentHandle
{
    Handle lighted;
    Handle rtImg;
}

public struct SSAOBlurHandle
{
    Handle ssaoBlur;
    Handle ssao;
    public int xBlurRange;
    public int yBlurRange;
}

public struct DDGIRadianceHandle
{
    Handle radianceMap;
    Handle depthMap;
    Handle tlas;
    Handle geometryNodeArray;
    Handle lightData;
    Handle skyboxTex;
    Handle probesArea;
}


