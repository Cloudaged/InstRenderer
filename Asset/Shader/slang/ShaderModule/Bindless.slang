module Bindless;

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

public struct PresentHandle
{
    public int lighted;
}


public struct ShadowSettings
{
    public float range;
}
