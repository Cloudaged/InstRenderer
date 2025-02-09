module DDGI;
import Types;

public struct ProbeArea
{
    public float4 position;
    public float4 normals[PROBE_NORMAL_COUNT];
}
public struct DDGIPayload
{
   public float3 radiance;
   public float depth;
}