module Bindless;

#define USING_UBO [[vk::binding(0, 0)]] uniform Sampler2D textures[];
#define USING_SSBO [[vk::binding(1, 0)]] uniform Sampler2D textures[];
#define USING_TEXTURE [[vk::binding(2, 0)]] uniform Sampler2D textures[];

public struct Material
{
   public int baseColor;
   public int normal;
   public int metallicRoughness;
}

public struct GeoHandles
{
    public int global;
    public Material material;
    public float4x4 modelMat;    
}

public struct GlobalUniform_B
{
    public float4 color;
}

public struct ShadowSettings
{
    public float range;
}
