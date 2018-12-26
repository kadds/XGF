
cbuffer CBMatrix:register(b0)
{
	row_major float4x4 cbWrold;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
};

cbuffer CBColor:register(b0)
{
	float4 cbColor;
};

struct VertexIn
{
	float3 Pos: POSITION;
	float3 Normal: NORMAL;
	float4 Color: COLOR;
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
	float3 Normal: NORMAL;
	float4 Color: COLOR;
};


float4 PS(VertexOut data) : SV_TARGET
{
	return cbColor *  data.Color;
}
VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos= mul(float4(data.Pos, 1.0f), cbWrold);
	vout.Pos = mul(vout.Pos, cbView);
	vout.Pos = mul(vout.Pos, cbProj);
	vout.Normal = data.Normal;
	vout.Color = data.Color;
	return vout;
}