module DDGI;
import Types;

public struct DDGIPayload
{
   public float3 radiance;
   public float distance;
   public float visibility;
}

public struct Probe
{
    public float4 position;
}

public int GetProbeIndex(int2 coords, int volumeSize)
{
    int y = coords.y / volumeSize;
    int z = coords.x / (volumeSize * PROBE_AREA_SIZE);
    int x = (coords.x - z * PROBE_AREA_SIZE * volumeSize) / volumeSize;

    return x + y * PROBE_AREA_SIZE + z * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
}

public float3 OctDecode(int2 coords, int volumeSize)
{
    float u = (2.0 * (coords.x / (volumeSize-1))) - 1.0;
    float v = (2.0 * (coords.y / (volumeSize-1))) - 1.0;

    float3 direction = float3(coords.x, coords.y, 1.f - abs(coords.x) - abs(coords.y));
    if (direction.z < 0.0)
    {
        // 反转x和y的符号
        direction.xy = (1.0 - abs(direction.yx)) * sign(direction.xy);
    }
    return normalize(direction);
}

public float3 FibonacciSphere(float sampleIndex, float numSamples)
{
    const float b = (sqrt(5.f) * 0.5f + 0.5f) - 1.f;
    float phi = PIx2 * frac(sampleIndex * b);
    float cosTheta = 1.f - (2.f * sampleIndex + 1.f) * (1.f / numSamples);
    float sinTheta = sqrt(saturate(1.f - (cosTheta * cosTheta)));

    return float3((cos(phi) * sinTheta), (sin(phi) * sinTheta), cosTheta);
}