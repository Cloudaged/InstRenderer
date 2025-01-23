module Types;
public static const float PI = 3.1415926;
public static const int CASCADED_COUNT = 4;

#define Handle public int
public struct Material
{
    Handle baseColor;
    Handle normal;
    Handle metallicRoughness;
}


public struct GlobalUniform
{
    public float4x4 viewMat;
    public float4x4 projMat;
    public float4x4 skyboxProjMat;
    public float4x4 lightViewMat;
    public float4x4 lightProjMat;
}

public struct CSMUniform
{
    public float4x4 vpMat[CASCADED_COUNT];
    public float4 cascadeSplits[CASCADED_COUNT];
    public float unitPerPix[CASCADED_COUNT];
    public int width;
    public int height;
    public float radiusBias;
}

public struct Light
{
    public float4 position;
    public float4 toward;
    public float4 color;
    public int type;
    public float intensity;
    public float range;
    public float lightSize;
    public float attenuation;
}

public struct LightCameraUniform
{
    public float4 cameraPos;
    public float4 cameraToward;
    public int lightCount;
    public Light light[16];
}

public struct RTUniform
{
    public float4x4 invView;
    public float4x4 invProj;
}

public struct ShadowSetting
{
    public const int pcfSampleCount;
    public const int blockerSearchCount;
    public const int showCascaded;
    const int antiShimmer;
    public const int enablePCF;
}

public struct RenderSetting
{
    public ShadowSetting shadowSetting;
}


public struct MaterialArr
{
   public Material materials[300];
}