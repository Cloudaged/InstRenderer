module DDGI;
import Types;

public struct DDGIPayload
{
   public float3 radiance;
   public float distance;
   public float visibility;
   public int curDepth;
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

public int2 GetProbeOffsetMapUV(int index)
{
    int z = index / (PROBE_AREA_SIZE * PROBE_AREA_SIZE);
    int temp = index - z * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
    int y = temp / PROBE_AREA_SIZE;
    int x = temp % PROBE_AREA_SIZE;

    int2 coords;
    coords.x = x + z * PROBE_AREA_SIZE;
    coords.y = y;

    return coords;
}

public float2 NormalizedOctCoords(int2 coordsInVolume)
{
    float2 octCoord = float2(coordsInVolume - 1) + float2(0.5);
    octCoord *= (2.0 / float(VOLUME_INTERIOR_SIZE));
    octCoord -= 1.0f;
    return octCoord;
}

public float3 OctDecode(float2 octCoord)
{
    float3 direction = float3(octCoord.x, octCoord.y, 1.f - abs(octCoord.x) - abs(octCoord.y));
    if (direction.z < 0.0)
    {
        direction.xy = (1.0 - abs(direction.yx)) * sign(direction.xy);
    }
    return normalize(direction);
}

public float2 OctEncode(float3 direction)
{
    direction.xy /= (abs(direction.x) + abs(direction.y) + abs(direction.z));

    if (direction.z < 0.0)
    {
        direction.xy = (1.0 - abs(direction.yx)) * sign(direction.xy);
    }

    // float2 coords;
    // coords.x = ((direction.x * 0.5 + 0.5) * (interiorSize-1));
    // coords.y = ((direction.y * 0.5 + 0.5) * (interiorSize-1));

    return direction.xy;
}

public float2 GetVolumeUV(float3 direction,int3 probeCoords)
{
    float2 octCoords = OctEncode(normalize(direction));
    float2 grid;
    grid.x = (probeCoords.x + probeCoords.z * PROBE_AREA_SIZE) * IRRADIANCE_VOLUME_SIZE;
    grid.y = probeCoords.y * IRRADIANCE_VOLUME_SIZE;
    grid += float2(1.0);//from [0,6] -> [1,7]
    grid += float2(VOLUME_INTERIOR_SIZE * 0.5);//Move to center pixel

    float2 irradianceTextureSize = float2(PROBE_AREA_SIZE * PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE, PROBE_AREA_SIZE * IRRADIANCE_VOLUME_SIZE);
    return (octCoords * float2(VOLUME_INTERIOR_SIZE * 0.5) + grid) / irradianceTextureSize;
}

public int GetProbeArrayIndex1D(int3 coords)
{
    return coords.x + PROBE_AREA_SIZE * coords.y + coords.z * PROBE_AREA_SIZE * PROBE_AREA_SIZE;
}

public int3 GetProbeCoords3D(int index)
{
    int z = index / (PROBE_AREA_SIZE * PROBE_AREA_SIZE);         
    int remaining = index % (PROBE_AREA_SIZE * PROBE_AREA_SIZE); 
    int y = remaining / PROBE_AREA_SIZE;                         
    int x = remaining % PROBE_AREA_SIZE;
    return int3(x, y, z);
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