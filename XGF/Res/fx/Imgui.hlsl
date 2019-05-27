Texture2D gShaderTexture;
SamplerState gSampleType;
cbuffer CBMatrix:register(b0)
{
	row_major float4x4 cbWVP;
};

struct VertexIn
{
	float2 Pos: POSITION;
	float2 Tex: TEXCOORD;
	float4 Color: COLOR;
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
	float2 Tex: TEXCOORD;
	float4 Color: COLOR;
};


float4 PS(VertexOut data) : SV_TARGET
{
	return gShaderTexture.Sample(gSampleType, data.Tex) *  data.Color;
}
VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos= mul(float4(data.Pos.xy, 0.0f, 1.0f), cbWVP);
	vout.Color = data.Color;
	vout.Tex = data.Tex;
	return vout;
}