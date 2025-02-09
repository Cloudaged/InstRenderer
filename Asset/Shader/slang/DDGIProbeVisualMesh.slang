struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 color    : COLOR0;
};

static const float4 positions[3] =
    {
        float4(-0.1, -0.1, 0.0,1.0),
        float4(0.1, -0.1, 0.0,1.0),
        float4(0.0, 0.1, 0.0,1.0)
    };

static const float3 colors[3] = {
    float3(1.0, 0.0, 0.0), // 红色
    float3(0.0, 1.0, 0.0), // 绿色
    float3(0.0, 0.0, 1.0)  // 蓝色
};

[numthreads(18, 6, 1)]
[shader("mesh")]
[outputtopology("triangle")]
void main(out vertices VertexOutput verts[3], out indices uint3 tris[1], uint3 DispatchThreadID: SV_DispatchThreadID)
{
        SetMeshOutputCounts(3, 1);

        for (int i = 0; i < 3; i++)
        {
            verts[i].position = positions[i];
            verts[i].color = DispatchThreadID;
        }
        tris[0] = uint3(0, 1, 2);
}
