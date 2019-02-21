
cbuffer CBMatrix:register(b0)
{
	row_major float4x4 cbViewProj;
};

struct VertexIn
{
	float3 Pos: POSITION;
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
};

VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos = mul(float4(data.Pos, 1.0f), cbViewProj);
	return vout;
}