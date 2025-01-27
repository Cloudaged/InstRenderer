module RayTracing;
import Types;

public struct Attributes
{
    public float2 att;
};

public struct MyPayload
{
    public float3 hitValue;
    public bool isInShadow;  
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