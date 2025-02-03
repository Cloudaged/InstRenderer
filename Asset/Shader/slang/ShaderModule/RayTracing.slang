module RayTracing;
import Types;

public struct Attributes
{
    public float2 att;
};

public struct MyPayload
{
    public float3 radiance;
    public float visibility;
    public int maxBounceTimes;
    public int curDepth;
    public int curSPP;
}

public struct Pixel
{
   public float3 position;
   public float3 normal;
   public float2 uv;
   public float3 tangent;
   public float3 bitangent;
}

public struct Face
{
    public Vertex vertices[3];
}

public struct RayDiff
{
   public float3 dOdx;
   public float3 dOdy;
   public float3 dDdx;
   public float3 dDdy;
}

public struct UVDiff
{
   public float2 duvdx;
   public float2 duvdy;
}



public struct Camera
{
   public float3 right;
   public float3 dir;
   public float3 up;
}

public UVDiff GetUVDifferentials(Face face,Camera cam,float rayT,float2 dims)
{
    UVDiff uvDiff;

    //Get edges and face normal
    float3 edge0 = face.vertices[1].position.xyz-face.vertices[0].position.xyz;
    float3 edge1 = face.vertices[2].position.xyz-face.vertices[0].position.xyz;
    float3 faceNormal = cross(edge0,edge1);
    
    RayDiff rayDiff;
    //Get ray direction differentials
    float dd = dot(cam.dir,cam.dir);
    float divd = 2.f/(dd*sqrt(dd));
    float dr = dot(cam.dir,cam.right);
    float du = dot(cam.dir,cam.up);
    rayDiff.dDdx = ((dd*cam.right)-(dr*cam.dir))*divd/dims.x;
    rayDiff.dDdy = -((dd*cam.up)-(du*cam.dir))*divd/dims.y;
    
    float3 dodx = float3(0) + rayT*rayDiff.dDdx;
    float3 dody = float3(0) + rayT*rayDiff.dDdy;

    float rcpDN = 1.f/dot(cam.dir,faceNormal);
    float dtdx = -dot(dodx,faceNormal)*rcpDN;
    float dtdy = -dot(dody,faceNormal)*rcpDN;
    dodx += cam.dir*dtdx;
    dody += cam.dir*dtdy;

    rayDiff.dOdx = dodx;
    rayDiff.dOdy = dody;

    //Get bary differentials
    float3 Nu = cross(edge1,faceNormal);
    float3 Nv = cross(edge0,faceNormal);

    float3 Lu = Nu/(dot(Nu,edge0));
    float3 Lv = Nv/(dot(Nv,edge1));
    float2 dBarydx,dBarydy;
    dBarydx.x = dot(Lu,rayDiff.dOdx);
    dBarydx.y = dot(Lv,rayDiff.dOdx);

    dBarydy.x = dot(Lu,rayDiff.dOdy);
    dBarydy.y = dot(Lv,rayDiff.dOdy);

    float2 delta1 = face.vertices[1].uv.xy - face.vertices[0].uv.xy;
    float2 delta2 = face.vertices[2].uv.xy - face.vertices[0].uv.xy;

    uvDiff.duvdx = dBarydx.x*delta1+dBarydx.y*delta2;
    uvDiff.duvdy = dBarydy.x * delta1 + dBarydy.y * delta2;
    return uvDiff;
}

uint WangHash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

uint Xorshift(uint seed)
{
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed;
}

uint MixSeed(uint s)
{
    s ^= s >> 17;
    s *= 0xed5ad4bb;
    s ^= s >> 11;
    s *= 0xac4c1b51;
    s ^= s >> 15;
    return s;
}

uint stepRNG(uint rngState)
{
    return rngState * 747796405 + 1;
}

// Steps the RNG and returns a floating-point value between 0 and 1 inclusive.
float stepAndOutputRNGFloat(inout uint rngState)
{
    // Condensed version of pcg_output_rxs_m_xs_32_32, with simple conversion to floating-point [0,1].
    rngState = stepRNG(rngState);
    uint word = ((rngState >> ((rngState >> 28) + 4)) ^ rngState) * 277803737;
    word = (word >> 22) ^ word;
    return float(word) / 4294967295.0f;
}

public float GetRandomNumber(inout uint seed)
{
    // seed = WangHash(seed);
    // return float(Xorshift(seed)) * (1.f / 4294967296.f);
    uint newSeed = MixSeed(seed);
    return stepAndOutputRNGFloat(newSeed);
}

// float a = GetRandomNumber(seed) * PI*2;
// float z = GetRandomNumber(seed) * 2.f - 1.f;
// float r = sqrt(1.f - z * z);

// float3 p = float3(r * cos(a), r * sin(a), z) + direction;
// return normalize(p);

public float3 MultipleSample(float3 normal, float3 viewDir, float roughness ,inout float cosWeight,inout float brdfWeight,inout uint seed)
{
    float random1 = GetRandomNumber(seed);
    float random2 = GetRandomNumber(seed);
    float phi = 2 * PI * random2;

    //Cosine
    float thetaC = acos(sqrt(1 - random1));
    float3 localDirC = float3(sin(thetaC) * cos(phi), sin(thetaC) * sin(phi), cos(thetaC));

    float3 upC = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangentC = normalize(cross(upC, normal));
    float3 bitangentC = cross(normal, tangentC);
    float3 outDirC = tangentC * localDirC.x + bitangentC * localDirC.y + normal * localDirC.z;

    // BRDF
    // float thetaB = atan(roughness * sqrt(random1 / (1 - random1)));
    float alpha2 = roughness * roughness;
    float thetaB = acos(sqrt((1 - random1) / (1 + (alpha2 - 1) * random1)));

    float3 localDirB = float3(sin(thetaB) * cos(phi), sin(thetaB) * sin(phi), cos(thetaB));
    float3 upB = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangentB = normalize(cross(upB, normal));
    float3 bitangentB = cross(normal, tangentB);

    float3 halfDir = tangentB * localDirB.x + bitangentB * localDirB.y + normal * localDirB.z;

    float3 outDirB = normalize(reflect(viewDir, halfDir));

    cosWeight = thetaC;
    brdfWeight = thetaB;
    float total = brdfWeight + cosWeight;
    // float3 finalDir = (cosWeight / total) * outDirC + (brdfWeight / total) * outDirB;
    float3 finalDir = outDirC;
    return finalDir;
}


public float3 GetRandomCosineDirectionOnHemisphere(float3 normal,inout uint seed)
{
    float random1 = GetRandomNumber(seed);
    float random2 = GetRandomNumber(seed);

    float theta = acos(sqrt(1 - random1));
    float phi = 2 * PI * random2;

    float3 localDir = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    float3 up = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);

    return normalize(tangent * localDir.x + bitangent * localDir.y + normal * localDir.z);
}

public float3 GetBrdfPDFDir(float3 normal,float3 viewDir,float roughness,inout uint seed)
{
    float random1 = GetRandomNumber(seed);
    float random2 = GetRandomNumber(seed);

    float theta = atan(roughness * sqrt(random1 / (1 - random1)));
    float phi = 2 * PI * random2;

    float3 localDir = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    float3 up = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);

    float3 halfDir = tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;

    float3 outDir = normalize(reflect(-viewDir, halfDir));
    return outDir;
    // 生成随机数
    // float random1 = GetRandomNumber(seed);
    // float random2 = GetRandomNumber(seed);

    // // GGX 采样（重要性采样）
    // float a2 = roughness * roughness;
    // float cosTheta = sqrt((1 - random1) / (1 + (a2 - 1) * random1));
    // float sinTheta = sqrt(1 - cosTheta * cosTheta);
    // float phi = 2 * PI * random2;

    // // 计算局部半向量
    // float3 localDir = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

    // // 计算正交基（TBN）
    // float3 up = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    // float3 tangent = normalize(cross(up, normal));
    // float3 bitangent = cross(normal, tangent);

    // // 转换到世界坐标系
    // float3 halfDir = tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;

    // // 计算最终采样方向
    // float3 outDir = reflect(-viewDir, halfDir); // 这里要确保 viewDir 指向表面

    // return outDir;
}

public Face GetFaceData(GeometryNode node,int primitiveID)
{
    Face face;
    VerticesArr *varr = (VerticesArr *)node.vertices;
    IndicesArr *iarr = (IndicesArr *)node.indices;

    uint3 verticesID = iarr.i[primitiveID];

    face.vertices[0] = varr.v[verticesID.x];
    face.vertices[1] = varr.v[verticesID.y];
    face.vertices[2] = varr.v[verticesID.z];
    return face;
}

public Pixel GetPixelData(Face face,float2 attribs)
{
    Pixel pixel;

    Vertex v1 = face.vertices[0];
    Vertex v2 = face.vertices[1];
    Vertex v3 = face.vertices[2];
    const float3 bary = float3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    for (int i = 0; i < 3; i++)
    {
        pixel.position = (bary.x * v1.position + bary.y * v2.position + bary.z * v3.position).xyz;
        pixel.normal = (bary.x * v1.normal + bary.y * v2.normal + bary.z * v3.normal).xyz;
        pixel.uv = (bary.x * v1.uv + bary.y * v2.uv + bary.z * v3.uv).xy;
        pixel.tangent = (bary.x * v1.tangent + bary.y * v2.tangent + bary.z * v3.tangent).xyz;
        pixel.bitangent = (bary.x * v1.bitangent + bary.y * v2.bitangent + bary.z * v3.bitangent).xyz;
    }
    return pixel;
}