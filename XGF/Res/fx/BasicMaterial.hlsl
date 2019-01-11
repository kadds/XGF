#ifdef USE_TEXTURE
Texture2D gShaderTexture;
SamplerState gSampleType;
#endif
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
#ifdef USE_NORMAL
	float3 Normal: NORMAL;
#endif
#ifdef USE_TEXTURE
	float2 Tex: TEXCOORD;
#endif
#ifdef USE_COLOR
	float4 Color: Color;
#endif
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
#ifdef USE_NORMAL
	float3 Normal: NORMAL;
#endif
#ifdef USE_TEXTURE
	float2 Tex: TEXCOORD;
#endif
#ifdef USE_COLOR
	float4 Color: Color;
#endif
};


float4 PS(VertexOut data) : SV_TARGET
{
	float4 c = cbColor;
#ifdef USE_TEXTURE
	c *= gShaderTexture.Sample(gSampleType, data.Tex);
#endif
#ifdef USE_COLOR
	c *= data.Color;
#endif
	return c;
}
VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos= mul(float4(data.Pos, 1.0f), cbWrold);
	vout.Pos = mul(vout.Pos, cbView);
	vout.Pos = mul(vout.Pos, cbProj);
#ifdef USE_NORMAL
	vout.Normal = data.Normal;
#endif
#ifdef USE_TEXTURE
	vout.Tex = data.Tex;
#endif
#ifdef USE_COLOR
	vout.Color = data.Color;
#endif
	return vout;
}