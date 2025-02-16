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

public int GetVolumeProbeArrayIndex(int2 coords, int volumeSize)
{
    int y = coords.y / volumeSize;
    int z = coords.x / (volumeSize * PROBE_AREA_SIZE);
    int x = (coords.x - z * PROBE_AREA_SIZE * volumeSize) / volumeSize;

    return x + y * PROBE_AREA_SIZE + z * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
}

public float3 OctDecode(int2 coords, int volumeSize)
{
    float u = (2.0 * (coords.x / (volumeSize - 1))) - 1.0;
    float v = (2.0 * (coords.y / (volumeSize - 1))) - 1.0;

    float3 direction = float3(coords.x, coords.y, 1.f - abs(coords.x) - abs(coords.y));
    if (direction.z < 0.0)
    {
        direction.xy = (1.0 - abs(direction.yx)) * sign(direction.xy);
    }
    return normalize(direction);
}

public float2 OctEncode(float3 direction, int volumeSize)
{
    direction.xy /= (abs(direction.x) + abs(direction.y) + abs(direction.z));

    if (direction.z < 0.0)
    {
        direction.xy = (1.0 - abs(direction.yx)) * sign(direction.xy);
    }

    int2 coords;
    coords.x = (int)((direction.x * 0.5 + 0.5) * (volumeSize - 1) + 0.5);
    coords.y = (int)((direction.y * 0.5 + 0.5) * (volumeSize - 1) + 0.5);

    return coords;
}

public float2 GetVolumeUV(float3 direction,int3 probeCoords)
{
    float2 octCoords = OctEncode(normalize(direction), IRRADIANCE_VOLUME_SIZE);
    float2 grid;
    grid.x = (probeCoords.x + probeCoords.z * PROBE_AREA_SIZE) * IRRADIANCE_VOLUME_SIZE;
    grid.y = probeCoords.y * IRRADIANCE_VOLUME_SIZE;
    float2 irradianceTextureSize = float2(PROBE_AREA_SIZE * PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE, PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE);
    return (octCoords * IRRADIANCE_VOLUME_SIZE + grid) / irradianceTextureSize;
}

public int GetProbeArrayIndex1D(int3 coords)
{
    return coords.x + PROBE_AREA_SIZE * coords.y + coords.z * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
}

public float3 FibonacciSphere(float sampleIndex, float numSamples)
{
    const float b = (sqrt(5.f) * 0.5f + 0.5f) - 1.f;
    float phi = PIx2 * frac(sampleIndex * b);
    float cosTheta = 1.f - (2.f * sampleIndex + 1.f) * (1.f / numSamples);
    float sinTheta = sqrt(saturate(1.f - (cosTheta * cosTheta)));

    return float3((cos(phi) * sinTheta), (sin(phi) * sinTheta), cosTheta);
}

public int3 GetGridIndice(float3 worldPos, float3 probeBasePos, float3 spacing)
{
    return clamp(int3((worldPos-probeBasePos)/spacing), int3(0), int3(PROBE_COUNT - 1));
}