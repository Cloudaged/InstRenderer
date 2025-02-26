module Types;
public static const float PI = 3.1415926;
public static const float PIx2 = 6.2831852;

public static const int CASCADED_COUNT = 4;

#define Handle public int
public static const int PROBE_AREA_SIZE = 16;
public static const int RAYS_PER_PROBE = 256;
public static const int IRRADIANCE_VOLUME_SIZE = 8;
public static const int VOLUME_INTERIOR_SIZE = IRRADIANCE_VOLUME_SIZE-2;

public static const int PROBE_COUNT = PROBE_AREA_SIZE * PROBE_AREA_SIZE * PROBE_AREA_SIZE;

public struct Vertex
{
    public float4 position;
    public float4 normal;
   public float4 uv;
   public float4 tangent;
   public float4 bitangent;
}

public struct VerticesArr
{
   public Vertex v[];
}

public struct IndicesArr
{
   public uint3 i[];
}

public struct GeometryNode
{
   public uint64_t vertices;
   public uint64_t indices;
   public uint64_t transform;
   public uint64_t pad0;
   Handle baseColor;
   Handle normal;
   Handle metallicRoughness;
   int pad1;
}

public struct Material
{
    Handle baseColor;
    Handle normal;
    Handle metallicRoughness;
    Handle pad0;
}


public struct GlobalUniform
{
    public float4x4 viewMat;
    public float4x4 projMat;
    public float4x4 skyboxProjMat;
    public float4x4 lightViewMat;
    public float4x4 lightProjMat;
    public float nearPlane;
    public float farPlane;
    public int isFirstFrame;
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

public struct SSAOKernel
{
   public float4 kernels[32];
}

public struct RTUniform
{
    public float4x4 invView;
    public float4x4 invProj;
    public float4 camRight;
    public float4 camUp;
}

public struct ShadowSetting
{
    public const int pcfSampleCount;
    public const int blockerSearchCount;
    public const int showCascaded;
    const int antiShimmer;
    public const int enablePCF;
    int pad0;
    int pad1;
    int pad2;
}

public struct DDGISetting
{
    public int showProbes;
    public int onlyIndirectLight;
    int pad0;
    int pad1;
}

public struct RenderSetting
{
    public ShadowSetting shadowSetting;
    public DDGISetting ddgiSetting;
}


